/*
 * Copyright (C) 2014  Bhaskar Kandiyal <bkandiyal@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "updatecommand.h"

#include <akonadi/itemfetchjob.h>
#include <akonadi/itemfetchscope.h>
#include <akonadi/itemmodifyjob.h>

#include <kcmdlineargs.h>
#include <kurl.h>
#include <qfile.h>

#include <iostream>

using namespace Akonadi;

UpdateCommand::UpdateCommand(QObject *parent)
    : AbstractCommand(parent),
      mDryRun(false),
      mFile(0)
{
    mShortHelp = ki18nc("@info:shell", "Updates an item's payload with the file specified").toString();
}

UpdateCommand::~UpdateCommand()
{
    delete mFile;
}

void UpdateCommand::setupCommandOptions(KCmdLineOptions &options)
{
    options.add("+item", ki18nc("@info:shell", "The item to update"));
    options.add("+file", ki18nc("@info:shell", "File to update the item from"));
    options.add(":", ki18nc("@info:shell", "Options for command"));
    options.add("n").add("dryrun", ki18nc("@info:shell", "Run command without making any actual changes"));
}

int UpdateCommand::initCommand(KCmdLineArgs *parsedArgs)
{
    if (parsedArgs->count() < 2) {
        emitErrorSeeHelp(ki18nc("@info:shell", "No item specified"));
        return InvalidUsage;
    }

    if (parsedArgs->count() == 2) {
        emitErrorSeeHelp(ki18nc("@info:shell", "No file specified"));
        return InvalidUsage;
    }

    mItemArg = parsedArgs->arg(1);
    mFileArg = parsedArgs->arg(2);
    mDryRun = parsedArgs->isSet("dryrun");

    return NoError;
}

void UpdateCommand::start()
{
    Item item;
    bool ok;
    int id = mItemArg.toInt(&ok);
    if (ok) {
        item = Item(id);
    } else {
        const KUrl url = QUrl::fromUserInput(mItemArg);
        if (url.isValid() && url.scheme() == QLatin1String("akonadi")) {
            item = Item::fromUrl(url);
        } else {
            emit error(i18nc("@info:shell", "Invalid item syntax '%1'", mItemArg));
            emit finished(RuntimeError);
            return;
        }
    }

    if (!QFile::exists(mFileArg)) {
        emit error(i18nc("@info:shell", "'%1' no such file or directory", mFileArg));
        emit finished(RuntimeError);
        return;
    }

    mFile = new QFile(mFileArg);
    if (!mFile->open(QIODevice::ReadOnly)) {
        emit error(i18nc("@info:shell", "Error opening file '%1' for reading", mFileArg));
        emit finished(RuntimeError);
        return;
    }

    ItemFetchJob *job = new ItemFetchJob(item, this);
    job->fetchScope().setFetchModificationTime(false);
    job->fetchScope().fetchAllAttributes(false);
    connect(job, SIGNAL(result(KJob *)), SLOT(onItemFetched(KJob *)));
}

void UpdateCommand::onItemFetched(KJob *job)
{
    if (job->error() != 0) {
        emit error(job->errorString());
        emit finished(RuntimeError);
        return;
    }

    ItemFetchJob *fetchJob = qobject_cast<ItemFetchJob *>(job);
    Q_ASSERT(fetchJob != 0);

    Item::List items = fetchJob->items();
    if (items.count() < 1) {
        emit error(i18nc("@info:shell", "No result returned for item '%1'", job->property("arg").toString()));
        emit finished(RuntimeError);
        return;
    }

    if (!mDryRun) {
        Item item = items.first();
        QByteArray data = mFile->readAll();
        item.setPayloadFromData(data);
        ItemModifyJob *modifyJob = new ItemModifyJob(item, this);
        connect(modifyJob, SIGNAL(result(KJob *)), SLOT(onItemUpdated(KJob *)));
    } else {
        onItemUpdated(job);
    }
}

void UpdateCommand::onItemUpdated(KJob *job)
{
    if (job->error() != 0) {
        emit error(job->errorString());
        emit finished(RuntimeError);
        return;
    }

    std::cout << i18nc("@info:shell", "Item updated successfully").toLocal8Bit().data() << std::endl;
    emit finished(NoError);
}
