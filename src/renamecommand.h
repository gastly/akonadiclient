/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  Bhaskar <email>
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

#ifndef RENAMECOMMAND_H
#define RENAMECOMMAND_H

#include <abstractcommand.h>

class CollectionResolveJob;
class KJob;

class RenameCommand : public AbstractCommand
{
  Q_OBJECT

  public:
    explicit RenameCommand(QObject* parent = 0);
    ~RenameCommand();

    QString name() const { return QLatin1String("rename"); }

  public Q_SLOTS:
    void start();

  protected:
    virtual int initCommand(KCmdLineArgs* parsedArgs);
    virtual void setupCommandOptions(KCmdLineOptions& options);

  private:
    CollectionResolveJob *mResolveJob;
    QString mCollectionNameArg;
    bool mDryRun;

  private Q_SLOTS:
    void onCollectionFetched( KJob *job );
    void onCollectionModified( KJob *job );
};

#endif // RENAMECOMMAND_H
