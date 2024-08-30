## Bootloaders in-depth guide 
- ### ON POWER BUTTON (Pre bootloader)
  - When this button is pressed, the wires connected to the button send an electronic signal to the motherboard. The motherboard simply reroutes this signal to 
 the power supply (PSU).
 The PSU then sends a signal, called the "power_good" signal into the motherboard to the Basic Input Output System (BIOS).   
  - BIOS recieves this "power_good" signal, the BIOS begins initializing a process called POST.   
  The POST then gives control to the BIOS. The POST loads the BIOS at the end of memory (Might be 0xFFFFF0) and puts a jump instruction at the first byte in memory.     
  BIOS will execute Interrupt (INT) 0x19 to attempt to find a bootable device.   
  If the BIOS found a bootable device it load what is on `0x7c00`
  
  - Our bootloader is multi staged, this means we need to call STAGE1 first.    
  To do so we put STAGE1 at `0x7c00` using `[org 0x7c00]` and when the BIOS jumps to `0x7c00` it should find our own STAGE 1!

- ### STAGE 1 bootloader
