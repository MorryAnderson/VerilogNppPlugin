#include "align.h"
using namespace Verilog;
#include <QTextStream>

Align::Align(){

}

void Align::Reset(){
    delimiters_.clear();
}

void Align::AddDelimeter(const char* str, int left_padding, int right_padding){
    delimiters_.append({QString(str), left_padding, right_padding});
}

int Align::GetAlignedCode(const QString& code, char** pointer, int indent){
    aligned_code_.clear();
    //
    QString code_str(code);
    QTextStream stream(&code_str);
    QString line;

    typedef QVector<QString> CellRow;
    typedef QVector<bool> DelimiterRow;

    QVector<CellRow> cell_table;
    QVector<DelimiterRow> delimiter_table;
    QString cell_str;

    int line_count = code_str.count('\n') + 1;
    cell_table.reserve(line_count);
    delimiter_table.reserve(line_count);

    int delimiter_num = delimiters_.size();
    DelimiterRow delimiter_exists(delimiter_num);
    QVector<int> delimeter_pos(delimiter_num);
    QVector<int> column_len(delimiter_num+1);

    int pos(-1), last_pos(0);
    int last_delimiter_len(0);
    int cell_index(0);
    int cell_start_pos(0);
    int cell_len(0);

    //! [analyse]
    while (!stream.atEnd()) {
        line = stream.readLine();
        cell_index = last_pos = last_delimiter_len = 0;
        cell_table.append(CellRow(delimiter_num+1));
        delimiter_table.append(DelimiterRow(delimiter_num));
        // analyse line
        for (int i=0; i<delimiters_.size(); ++i) {
            pos = line.indexOf(delimiters_.at(i).str, last_pos);
            delimeter_pos[i] = pos;
            if (pos != -1) {
                delimiter_exists[i] = true;
                cell_start_pos = last_pos + last_delimiter_len;
                cell_len = pos - cell_start_pos;
                if (cell_len < 0) cell_len = 0;
                cell_str = line.mid(cell_start_pos, cell_len).trimmed();
                cell_len = cell_str.length();
                column_len[cell_index] = qMax(column_len.at(cell_index), cell_len);
                // update table
                cell_table.last()[cell_index] = cell_str;
                delimiter_table.last()[i] = true;
                //
                cell_index = i + 1;
                last_pos = pos;
                last_delimiter_len = delimiters_.at(i).str.length();
            }
        }
        // last cell in row
        cell_start_pos = last_pos + last_delimiter_len;
        cell_len = line.length() - cell_start_pos;
        cell_str = line.right(cell_len).trimmed();
        cell_table.last()[cell_index] = cell_str;
        cell_len = cell_str.length();
        column_len[cell_index] = qMax(column_len.at(cell_index), cell_len);
    }
    //! [analyse]

    //! [calc row len]
    int row_len = indent;
    foreach (int len, column_len) {
        row_len += len;
    }
    for (int i=0; i<delimiters_.size(); ++i) {
        if (delimiter_exists.at(i)) {
            row_len += delimiters_.at(i).str.length() + delimiters_.at(i).left_padding
                                                      + delimiters_.at(i).right_padding;
        }
    }
    row_len += 2;  // '\r\n'
    //! [calc row len]

    //! [generate aligned code]
    line_count = cell_table.size();
    aligned_code_.reserve(row_len * cell_table.size() + 1);
    for (int row=0; row<line_count; ++row) {
        aligned_code_ += QString(indent, ' ');
        for (int col=0; col<delimiter_num; ++col) {
            aligned_code_ += cell_table[row][col].leftJustified(column_len.at(col));
            if (delimiter_exists.at(col)) {
                aligned_code_ += QString(delimiters_[col].left_padding, ' ');
                if (delimiter_table[row][col] == true) {
                    aligned_code_ += delimiters_[col].str;
                } else {
                    aligned_code_ += QString(delimiters_[col].str.length(), ' ');
                }
                aligned_code_ += QString(delimiters_[col].right_padding, ' ');
            }
        }
        aligned_code_ += cell_table[row][delimiter_num].leftJustified(column_len.at(delimiter_num));
        aligned_code_ += "\r\n";
    }
    aligned_code_.chop(2);
    //! [generate aligned code]
    //
    aligned_code_utf8_ = aligned_code_.toUtf8();
    if (pointer) *pointer = aligned_code_utf8_.data();
    return aligned_code_utf8_.length();
}

int Align::AlignVariableDecl(const char* code, char** pointer, int indent){
    aligned_code_.clear();
    int left_padding = delimiters_.at(0).left_padding;
    int right_padding = delimiters_.at(2).right_padding;
    //
    QString code_str(code);
    const QRegExp range_regexp("\\[.*:.*\\]");
    int range_str_index = code_str.indexOf(range_regexp);
    int equal_str_index = code_str.indexOf("=");
    bool contains_range(false);
    // range "[:]" should be before '='
    if (-1 != range_str_index) {  // no [:]
        if (-1 == equal_str_index) contains_range = true;
        else contains_range = range_str_index < equal_str_index;
    }
    //
    if (!contains_range) {
        delimiters_[0].left_padding = 0;
        delimiters_[2].right_padding = 0;
    }
    code_str.replace(QRegExp("(wire|reg)\\s+(signed|unsigned)?\\s*(\\w+)"), "\\1 \\2 [:] \\3");
    GetAlignedCode(code_str, pointer, indent);

    if (contains_range) {
        aligned_code_.replace(QRegExp("\\[(\\s*):(\\s*)\\]"), " \\1 \\2 ");
    } else {
        aligned_code_.replace(QRegExp("\\[(\\s*):(\\s*)\\]"), " ");
    }
    //
    delimiters_[0].left_padding = left_padding;
    delimiters_[2].right_padding = right_padding;
    aligned_code_utf8_ = aligned_code_.toUtf8();
    if (pointer) *pointer = aligned_code_utf8_.data();
    return aligned_code_utf8_.length();
}
