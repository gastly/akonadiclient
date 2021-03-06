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

#ifndef EDITCOMMAND_H
#define EDITCOMMAND_H

#include "abstractcommand.h"

class KJob;
class QTemporaryFile;

class EditCommand : public AbstractCommand
{
    Q_OBJECT

public:
    explicit EditCommand(QObject *parent = 0);
    ~EditCommand();

    QString name() const {
        return (QLatin1String("edit"));
    }

public Q_SLOTS:
    void start();

protected:
    int initCommand(KCmdLineArgs *parsedArgs);
    void setupCommandOptions(KCmdLineOptions &options);

private:
    QString mItemArg;
    QTemporaryFile *mTempFile;
    bool mDryRun;

private Q_SLOTS:
    void onItemFetched(KJob *job);
    void onItemModified(KJob *job);
};

#endif // EDITCOMMAND_H
