# How to install this plugin

%User% = your user directory in Windows
%Notepad++% = installation directory of Notepad++

> **this plugin is 64-bit**, if you want to use 32-bit version, remember to
> **rename** `VerilogNppPlugin_32bit.dll` & `Qt5Core_32bit.dll`
>            to `VerilogNppPlugin.dll` & `Qt5Core.dll`

1. Put `verilog.ini` in `%User%\AppData\Roaming\Notepad++\plugins\config\\`
2. Put `VerilogNppPlugin.dll` and `QtCore5.dll` in `%Notepad++%\plugins\VerilogNppPlugin`
3. Open a verilog source file, click **“Plugin -> Verilog HDL -> Copy Instantiation”** in Notepad++ menu, then paste

this plugin does not support “verilog-1995 style” declaration of module
for example: write
`module and(input a, input b, output q);`
instead of
`module and(a,b,q); input a, b; output q;`

if you are using **portable** version, or `%User%\AppData\Roaming\Notepad++\plugins` does not exists,
you need to put `verilog.ini` into `%Notepad++%\plugins`

# Commands

You can run these commands **without** selecting any text in your code.

| command             | description                                                  |
| ------------------- | ------------------------------------------------------------ |
| AutoComplete        | When turned on, auto complete keywords of Verilog HDL when typing. |
| Replace Declaration | Format the declaration code of current module                |
| Copy Instantiation  | Copy the instantiation code of current module into clipboard |
| Create Testbench    | Create a testbench template of current module in a new file  |
| Auto Align          | Align code by "=" **around the cursor**                      |

