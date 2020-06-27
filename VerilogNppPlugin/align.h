#ifndef ALIGN_H
#define ALIGN_H

#include <QVector>
#include <QString>

namespace Verilog {

class Align {
  public:
    Align();
    void Reset();
    void AddDelimeter(const char*, int left_padding = 0, int right_padding = 0);
    int GetAlignedCode(const char*, char**, int indent = 0);

  private:
    struct Delimeter {
        QString str;
        int left_padding, right_padding;
    };
    QVector<Delimeter> delimiters_;
    QString aligned_code_;
    QByteArray aligned_code_utf8_;
};

}

#endif // ALIGN_H
