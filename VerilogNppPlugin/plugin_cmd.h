#ifndef PLUGIN_CMD_H
#define PLUGIN_CMD_H

void Enabled();
void ReplaceModuleDeclaration();

// private functions:

void SetEnabled(bool);
void TrackLangName();
int RetrieveModuleBlock(char **, int* start = nullptr, int* end = nullptr);
bool RetrieveAndParseModule(int* start, int* end);

#endif // PLUGIN_CMD_H
