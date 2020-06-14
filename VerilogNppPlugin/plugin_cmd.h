#ifndef PLUGIN_CMD_H
#define PLUGIN_CMD_H

void Enabled();
void Module();

// private functions:

void SetEnabled(bool);
void TrackLangName();
int RetrieveModuleBlock(char **, int* start = nullptr, int* end = nullptr);

#endif // PLUGIN_CMD_H
