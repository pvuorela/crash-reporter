/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2013 Jolla Ltd.
 * Contact: Jakub Adam <jakub.adam@jollamobile.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "crashreporteradapter.h"

#include <QFileSystemWatcher>

#include "creportercoreregistry.h"
#include "creporterutils.h"

class CrashReporterAdapterPrivate {
public:
    CrashReporterAdapterPrivate(CrashReporterAdapter *qq);

    CReporterCoreRegistry registry;
    int reportsToUpload;

private:
    void countCrashReports();

    QFileSystemWatcher watcher;

    Q_DECLARE_PUBLIC(CrashReporterAdapter);
    CrashReporterAdapter *q_ptr;
};

CrashReporterAdapterPrivate::CrashReporterAdapterPrivate(CrashReporterAdapter *qq):
  reportsToUpload(0), q_ptr(qq)
{
    Q_Q(CrashReporterAdapter);

    countCrashReports();
    // Recalculate the reports left to upload when directory contents change.
    QObject::connect(&watcher, SIGNAL(directoryChanged(const QString&)),
                     q, SLOT(countCrashReports()));

    watcher.addPaths(*registry.getCoreLocationPaths());
}

void CrashReporterAdapterPrivate::countCrashReports()
{
    Q_Q(CrashReporterAdapter);

    int newCoresToUpload = registry.collectAllCoreFiles().count();
    if (newCoresToUpload != reportsToUpload) {
        reportsToUpload = newCoresToUpload;
        emit q->reportsToUploadChanged();
    }
}

CrashReporterAdapter::CrashReporterAdapter(QObject *parent):
  QObject(parent), d_ptr(new CrashReporterAdapterPrivate(this)) {}

int CrashReporterAdapter::reportsToUpload() const
{
    Q_D(const CrashReporterAdapter);

    return d->reportsToUpload;
}

void CrashReporterAdapter::uploadAllCrashReports() const
{
    Q_D(const CrashReporterAdapter);

    CReporterUtils::notifyAutoUploader(d->registry.collectAllCoreFiles());
}

#include "moc_crashreporteradapter.cpp"