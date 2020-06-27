#ifndef PLUGIN_CMD_H
#define PLUGIN_CMD_H

void Enabled();
void ReplaceModuleDeclaration();
void CopyInstantiationTemplate();
void CreateTestbench();
void AutoAlign();

// private functions:
typedef bool (*CheckLineFunc)(int start_pos, int end_pos);
typedef int (*ProcFunc)(const char*, char**, int);

void SetEnabled(bool);
void TrackLangName();
int RetrieveModuleBlock(char **, int* start = nullptr, int* end = nullptr);
bool RetrieveAndParseModule(int* start = nullptr, int* end = nullptr);
void ReplaceLines(CheckLineFunc, ProcFunc);
bool LineStartsWithDot(int start_pos, int end_pos);
bool LineContainsAssignment(int start_pos, int end_pos);
bool LineContainsUnblockingAssignment(int start_pos, int end_pos);
bool LineStartsWithRegOrWire(int start_pos, int end_pos);
int AlignPortList(const char*, char**, int indent = 0);
int AlignAssignment(const char*, char**, int indent = 0);
int AlignUnblockingAssignment(const char*, char**, int indent = 0);

#endif // PLUGIN_CMD_H
