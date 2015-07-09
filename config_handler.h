#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H

#include <QString>

/* reads and writes the configuration and state file ~/.ecdbgui
 * TODO modify so that it records surface (and other) id numbers
 * TODO modify so that it records the surface (and) list.
 * TODO add font,
 */

class ConfigHandler
{
public:
    ConfigHandler();
    void save() const; //saves currnet configuration
    int getLastSurf() const; //returns last surface id
    void setLastSurf(const int); //set last surface id
    void processLine(const QString line); //processes line of config file
private:
    int lastSurf;
};

#endif // CONFIG_HANDLER_H
