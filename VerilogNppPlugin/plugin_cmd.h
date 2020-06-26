#ifndef PLUGIN_CMD_H
#define PLUGIN_CMD_H

void Enabled();
void ReplaceModuleDeclaration();
void CopyInstantiationTemplate();
void CreateTestbench();
void AlignPortList();

// private functions:

void SetEnabled(bool);
void TrackLangName();
int RetrieveModuleBlock(char **, int* start = nullptr, int* end = nullptr);
bool RetrieveAndParseModule(int* start = nullptr, int* end = nullptr);
char GetFirstCharInLine(int line);

#endif // PLUGIN_CMD_H
