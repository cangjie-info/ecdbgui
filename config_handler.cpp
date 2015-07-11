#include "config_handler.h"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include "db_handler.h"

ConfigHandler::ConfigHandler()
    : lastSurfId(0)
{
    font = "ICS3";
    //check to see whether config file exists
    QDir home = QDir().home();
    QFile configFile;
    QDir::setCurrent(home.path());
    configFile.setFileName(".ecdbgui");
    if(configFile.exists())
    {
        //read it
        configFile.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream config(&configFile);
        QString line(config.readLine());
        while(!line.isNull())
        {
            processLine(line);
            line = config.readLine();
        }
    }
    else
    {
        //make it
        configFile.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream config(&configFile);
        config << "lastSurf=0\n";
        config << "font=ICS3";
        font = "ICS3";
        config << "fontList=ICS3,HuaDong,ICS4";
        fonts << "ICS3" << "HuaDong" << "ICS4";
        configFile.close();
    }
}

void ConfigHandler::save() const
{
    QDir home = QDir().home();
    QDir::setCurrent(home.path());
    QFile::remove(".ecdbgui_old");
    QFile::rename(".ecdbgui", ".ecdbgui_old");
    QFile configFile;
    configFile.setFileName(".ecdbgui");
    configFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream config(&configFile);

    config << QString("lastSurf=%1\n")
            .arg(lastSurfId);
    config << QString("font=%1\n")
              .arg(font);
    config << QString("fontList=%1\n")
              .arg(fonts.join(","));
    //and the rest - TODO
}

void ConfigHandler::processLine(QString line)
{
    QStringList nameValue = line.split("=");
    if(nameValue[0] == "lastSurf")
        lastSurfId = nameValue[1].toInt();
    else if(nameValue[0] == "font")
       font = nameValue[1];
    else if(nameValue[0] == "fontList")
       fonts = nameValue[1].split(",");
    //other config varialbles
}

QString ConfigHandler::getFont() const
{
   return font;
}

void ConfigHandler::setFont(QString newFont)
{
   font = newFont;
}

QStringList ConfigHandler::getFontList() const
{
   return fonts;
}

int ConfigHandler::getLastSurf() const
 {
     return lastSurfId;
 }

void ConfigHandler::setLastSurf(const int index)
 {
     lastSurfId = index;
 }
