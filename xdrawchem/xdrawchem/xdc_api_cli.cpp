// xdc_api_cli.cpp — `xdrawchem --api <command> [options]`
//
// This is the boundary between the argv-driven CLI and XdcApiResult, which
// carries structured output.  Commands are dispatched in main.cpp; this file
// only holds the option struct and argument table plus the entry point
// called from main.

#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>

#include "xdc_api.h"

// Dispatch an `--api` invocation.  `args` is the argv tail after "--api".
// Prints JSON to stdout (via caller) and returns the process exit code.
int apiMain( const QStringList &args )
{
    int code = 0;
    QString json = apiDispatch( args, &code );
    fputs( json.toUtf8().constData(), stdout );
    fputc( '\n', stdout );
    fflush( stdout );
    return code;
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
