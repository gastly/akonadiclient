/*
    Copyright (C) 2012  Kevin Krammer <krammer@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "collectionpathjob.h"

#include <KLocale>

#include <Akonadi/CollectionFetchJob>

#include <akonadi/private/collectionpathresolver_p.h>

using namespace Akonadi;


CollectionPathJob::CollectionPathJob(const Akonadi::Collection &collection, QObject *parent )
  : KCompositeJob(parent),
    mCollection(collection)
{
  setAutoDelete( false );
}

CollectionPathJob::~CollectionPathJob()
{
}


void CollectionPathJob::start()
{
  if (!mCollection.isValid())
  {
    emitResult();
    return;
  }

  CollectionPathResolver *resolver = new CollectionPathResolver(mCollection, this);
  addSubjob(resolver);
  resolver->start();
}


void CollectionPathJob::slotResult(KJob *job)
{
  if (job->error()==0)
  {
    CollectionPathResolver *resolver = qobject_cast<CollectionPathResolver *>(job);
    Q_ASSERT(resolver!=0);
    mPath = resolver->path();
  }

  bool willEmitResult = (job->error() && !error());
  KCompositeJob::slotResult(job);

  if (!hasSubjobs() && !willEmitResult)
  {
    emitResult();
  }
}


QString CollectionPathJob::collectionPath() const
{
  return (mPath);
}


QString CollectionPathJob::formattedCollectionPath() const
{
  return (i18nc("@info:shell 1=collection ID, 2=collection path",
                "%1 (\"/%2\")", QString::number(mCollection.id()), mPath));
}
