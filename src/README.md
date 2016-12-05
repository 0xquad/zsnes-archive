Debugger branch for ZSNES
=========================


The debugger branch is based on SVN r5100 and integrates an old patch of mine
which adds basic debugging capabilities to ZSNES.

Debug functions include

* set breakpoints on memory reads and writes
* dump memory
* disassemble
* display the call stack at breakpoints

It works by loading the file `/tmp/zsnes-bp.ini` which specifies one breakpoint
per line. It can be reloaded at runtime by pressing the `~` key from the ZSNES
screen (when the game is not running or is paused).

The file format is simple:

    # comments start with a pound sign
    # breakpoint format is:
    # BANK:ADDRESS:{r,w,rw}
    # example: break when the address 7e8ee1 is written to
    7e:8ee1:w

The debugger consists of a simple command line interface, so in this case make sure that ZSNES is started from a terminal to have an interactive stdin.

In the debugger environment, there is a simple help menu that will let you
manage breakpoints dynamically, dump the RAM, disassemble the code and display
the call stack.

The call stack becomes essential to figure out almost exactly where the
operation occurs, because ZSNES does not seem to update the Instruction Pointer
at every executed instruction, but rather only when jumps are taken (calls,
near jumps, far jumps, returns, etc.). Because of this, the IP displayed when a
breakpoint is hit is not always where the instruction has performed the
monitored operation; the user often has to find the correct point by reading
instructions back (in a separate disassembly of the ROM file).


Limitations
-----------

* The address displayed in the debugger when a breakpoint occurs is not accurate.
* The `autogen.sh` fails today with the newer autotools and development tools
  (some options are not recognized anymore, etc.).
