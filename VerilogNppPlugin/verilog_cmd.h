#ifndef VERILOG_CMD_H
#define VERILOG_CMD_H

class VerilogCmd {
  public:
    VerilogCmd();
    inline bool get_enabled(){return enabled_;}
    inline void set_enabled(bool state){enabled_ = state;}
  private:
    bool enabled_;
};

#endif // VERILOG_CMD_H
