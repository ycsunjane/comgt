/***************************************************************************
 *  comgt.h - 3G/GPRS datacard management utility header file
 *
 *  Copyright (C) 2003  Paul Hardwick <paul@peck.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ***************************************************************************/

/***************************************************************************
* $Id: comgt.h,v 1.4 2006/10/20 14:30:19 pharscape Exp $
****************************************************************************/
#define COMGT_VERSION "0.32"
#define YEARS "2005,2006"

char _default_code[] =
    "opengt\r\n\
    set com 115200n81\r\n\
    set senddelay 0.05\r\n\
    send \"AT+CFUN=1^m\"\r\n\
    waitquiet 1 0.2\r\n\
  :start\r\n\
    flash 0.1\r\n\
    send \"AT+CPIN?^m\"\r\n\
    waitfor 30 \"SIM PUK\",\"SIM PIN\",\"READY\",\"ERROR\",\"ERR\"\r\n\
    if % = -1 goto error\r\n\
    if % = 0 goto ready\r\n\
    if % = 1 goto getpin\r\n\
    if % = 2 goto ready\r\n\
    if % = 3 goto error\r\n\
    if % = 4 goto error\r\n\
  :error\r\n\
    print $s,\" ***SIM ERROR***\r\n\"\r\n\
    print \"Check device port configuration.\r\nCheck SIM is inserted\r\nTest SIM in a mobile phone?\r\n\"\r\n\
    exit 1\r\n\
  :getpin\r\n\
    #handle case where original Vodafone 3G generates wrong response\r\n\
    waitfor 1 \"2\"\r\n\
    if % = 0 goto ready\r\n\
    print \"\r\nEnter PIN number: \"\r\n\
    input $x\r\n\
    let a=len($x)\r\n\
    if a<>5 goto getpin\r\n\
    let c=0\r\n\
  :test\r\n\
    let $c=$mid($x,c,1)\r\n\
    if $c<\"0\" goto getpin\r\n\
    if $c>\"9\" goto getpin\r\n\
    inc c\r\n\
    if c<4 goto test\r\n\
    let a=val($x)\r\n\
    if a<0 goto getpin\r\n\
    if a>9999 goto getpin\r\n\
    let $c=$left($x,4)\r\n\
  :enterpin\r\n\
    send \"AT+CPIN=\\\"\"\r\n\
    send $c\r\n\
    send \"\\\"^m\"\r\n\
    waitfor 20 \"OK\",\"ERR\"\r\n\
    if % = -1 goto timeerror\r\n\
    if % = 0 goto cont\r\n\
    if % = 1 goto pinerror\r\n\
  :pinerror\r\n\
  :timeerror\r\n\
    print \"ERROR entering PIN code\r\n\"\r\n\
    print \"Caution! - entering the wrong PIN code three times will lock the SIM\r\n\"\r\n\
    exit 1\r\n\
  :ready\r\n\
    print \"SIM ready\r\n\"\r\n\
  :cont\r\n\
    print \"Waiting for Registration..(120 sec max)\"\r\n\
    let c = 0\r\n\
  :waitreg\r\n\
    send \"AT+CREG?^m\"\r\n\
    waitfor 2 \"+CREG: 0,1\",\"+CREG: 0,5\"\r\n\
    if % = -1 goto regagain\r\n\
    if % = 0 goto homereg\r\n\
    if % = 1 goto roamreg\r\n\
  :regagain\r\n\
    if c > 120 goto regtimeout\r\n\
    let c=c+2\r\n\
    print \".\"\r\n\
    goto waitreg\r\n\
  :regtimeout\r\n\
    print \"\r\nFailed to register\r\n\"\r\n\
    exit 1\r\n\
  :homereg\r\n\
    print \"\r\nRegistered on Home network: \"\r\n\
    goto registered\r\n\
  :roamreg\r\n\
    print \"\r\nRegistered on Roaming network: \"\r\n\
    goto registered\r\n\
  :registered\r\n\
    waitquiet 1 0.1\r\n\
    send \"AT+COPS?^m\"\r\n\
    get 2 \"^m\" $s\r\n\
    get 2 \"^m\" $s\r\n\
    let a=len($s)\r\n\
    let b=a-12\r\n\
    if b < 1 goto regtimeout\r\n\
    let $c=$right($s,b)\r\n\
    print $c,\"\r\n\"\r\n\
    let c=0\r\n\
  :signal\r\n\
    waitquiet 1 0.1\r\n\
    send \"AT+CSQ^m\"\r\n\
    get 2 \"^m\" $s\r\n\
    get 2 \"^m\" $s\r\n\
    let a=len($s)\r\n\
    let a=a-6\r\n\
    let $s=$right($s,a)\r\n\
    if $s <> \"0,0\" goto sigcont\r\n\
    if c > 3 goto sigexit\r\n\
    let c=c+1\r\n\
    pause 1\r\n\
    goto signal\r\n\
  :sigexit\r\n\
    print \"Signal strength measure 0,0 too low!\"\r\n\
    exit 1\r\n\
  :sigcont\r\n\
    print \"Signal Quality:\",$s,\"\\r\n\"\r\n\
    waitquiet 1 0.1\r\n";

char _info_code[]  =
    "print \"##### Wireless WAN Modem Configuration #####\\r\n\"\r\n\
    opengt\r\n\
    set com 115200n81\r\n\
    set senddelay 0.05\r\n\
    waitquiet 2 0.5\r\n\
  :name\r\n\
    print \"Product text:\r\n====\r\n\"\r\n\
    send \"ATi^m\"\r\n\
    get 1 \" ^m\" $s\r\n\
  :more\r\n\
    get 1 \"^m\" $s\r\n\
    let x=len($s)\r\n\
    if $s=\"OK\r\n\" goto manf\r\n\
    #dump\r\n\
    if x<1 goto manf\r\n\
    if $s=\"\r\n\" goto more\r\n\
    print $s\r\n\
    goto more\r\n\
  :manf\r\n\
    print \"\r\n====\r\n\"\r\n\
    waitquiet 2 0.1\r\n\
    print \"Manufacturer:           \"\r\n\
    send \"AT+cgmi^m\"\r\n\
    get 1 \"^m\" $s\r\n\
    get 1 \"^m\" $s\r\n\
    let x=len($s)\r\n\
    if x<1 goto imei_serial\r\n\
    dec x\r\n\
    let $s=$right($s,x)\r\n\
    print $s,\"\\r\n\"\r\n\
  :imei_serial\r\n\
    waitquiet 5 0.1\r\n\
    print \"IMEI and Serial Number: \"\r\n\
    send \"AT+GSN^m\"\r\n\
    get 2 \"^m\" $s\r\n\
    get 2 \"^m\" $s\r\n\
    let x=len($s)\r\n\
    dec x\r\n\
    let $s=$right($s,x)\r\n\
    print ,$s,\"\\r\n\"\r\n\
  :firmware\r\n\
    waitquiet 5 0.1\r\n\
    print \"Manufacturer\'s Revision: \"\r\n\
    send \"AT+GMR^m\"\r\n\
    get 2 \"^m\" $s\r\n\
    get 2 \"^m\" $s\r\n\
    let x=len($s)\r\n\
    dec x\r\n\
    dec x\r\n\
    let $s=$left($s,x)\r\n\
    print $s,\"\\r\n\"\r\n\
  :hardware\r\n\
    waitquiet 5 0.1\r\n\
    print \"Hardware Revision:      \"\r\n\
    send \"AT_OHWV^m\"\r\n\
    get 2 \" ^m\" $s\r\n\
    get 2 \" ^m\" $s\r\n\
    get 2 \" ^m\" $s\r\n\
    print $s,\"\\r\n\"\r\n\
  :networklock\r\n\
    waitquiet 5 0.1\r\n\
    print \"Network Locked:         \"\r\n\
    send \"AT+clck=\\\"PN\\\",2^m\"\r\n\
    get 2 \" ^m\" $s\r\n\
    get 2 \" ^m\" $s\r\n\
    get 2 \" ^m\" $s\r\n\
    print $s,\"\\r\n\"\r\n\
    waitquiet 5 0.1\r\n\
  :customized\r\n\
    print \"Customisation:          \"\r\n\
    send \"AT_ocst^m\"\r\n\
    get 2 \" ^m\" $s\r\n\
    get 2 \" ^m\" $s\r\n\
    get 2 \",^m\" $s\r\n\
    print $s,\"\\r\n\"\r\n\
  :bandsettings\r\n\
    waitquiet 5 0.1\r\n\
    print \"Band settings:          \"\r\n\
    send \"AT_OSBM?^m\"\r\n\
    get 2 \" ^m\" $s\r\n\
    get 2 \" ^m\" $s\r\n\
    get 2 \" ^m\" $s\r\n\
    if $s=\"4\" print \"Europe 900/1800MHz \"\r\n\
    if $s=\"5\" print \"USA 900/1900MHz \"\r\n\
    print \"(\",$s,\")\\r\n\" \r\n\
  :apn\r\n\
    waitquiet 5 0.1\r\n\
    print \"APN:                    \"\r\n\
    send \"AT+CGDCONT?^m\"\r\n\
    get 2 \" ^m\" $s\r\n\
    get 2 \" ^m\" $s\r\n\
    get 2 \" ^m\" $s\r\n\
    print $s,\"\\r\n\" \r\n\
    waitquiet 5 0.1  \r\n\
    print \"##### END #####\\r\n\"" ;


char _USA_code[]=
    "print \"##### Band Change to USA operation #####\\r\n\"\r\n\
    opengt\r\n\
    set com 115200n81\r\n\
    set senddelay 0.05\r\n\
    waitquiet 2 0.5\r\n\
    send \"AT_OSBM=5^m\"\r\n\
    waitfor 10 \"OK\",\"ERR\"\r\n\
    if % = -1 goto timeout\r\n\
    if % = 0 goto cont\r\n\
    if % = 1 goto error\r\n\
  :timeout\r\n\
    print \"Timeout Error communicating with device.\r\n\"\r\n\
    exit 1\r\n\
  :error\r\n\
    print \"Error response from device.\r\n\"\r\n\
    exit 1\r\n\
  :cont\r\n\
    print \"Complete\\r\n\"\r\n";

char _Europe_code[]  =
    "print \"##### Band Change to European operation #####\\r\n\"\r\n\
    opengt\r\n\
    set com 115200n81\r\n\
    set senddelay 0.05\r\n\
    waitquiet 2 0.5\r\n\
    send \"AT_OSBM=4^m\"\r\n\
    waitfor 10 \"OK\",\"ERR\"\r\n\
    if % = -1 goto timeout\r\n\
    if % = 0 goto cont\r\n\
    if % = 1 goto error\r\n\
  :timeout\r\n\
    print \"Timeout Error communicating with device.\r\n\"\r\n\
    exit 1\r\n\
  :error\r\n\
    print \"Error response from device.\r\n\"\r\n\
    exit 1\r\n\
  :cont\r\n\
    print \"Complete\\r\n\"\r\n";

char _sig_code[]  =
    "opengt\r\n\
    set com 115200n81\r\n\
    set senddelay 0.05\r\n\
    waitquiet 2 0.5\r\n\
    let c=0\r\n\
  :signal\r\n\
    waitquiet 1 0.1\r\n\
    send \"AT+CSQ^m\"\r\n\
    get 2 \"^m\" $s\r\n\
    get 2 \"^m\" $s\r\n\
    let a=len($s)\r\n\
    let a=a-6\r\n\
    let $s=$right($s,a)\r\n\
    if $s <> \"0,0\" goto sigcont\r\n\
    if c > 3 goto sigexit\r\n\
    let c=c+1\r\n\
    pause 1\r\n\
    goto signal\r\n\
  :sigexit\r\n\
    print \"Signal strength measure 0,0 too low!\"\r\n\
    exit 1\r\n\
  :sigcont\r\n\
    print \"Signal Quality:\",$s,\"\\r\n\"\r\n\
    waitquiet 1 0.1\r\n\
    exit 0\r\n";

char _reg_code[]  =
    "opengt\r\n\
    set com 115200n81\r\n\
    set senddelay 0.05\r\n\
    waitquiet 2 0.5\r\n\
    print \"Waiting for Registration\"\r\n\
    let c = 0\r\n\
  :waitreg\r\n\
    send \"AT+CREG?^m\"\r\n\
    waitfor 2 \"+CREG: 0,1\",\"+CREG: 0,5\"\r\n\
    if % = -1 goto regagain\r\n\
    if % = 0 goto homereg\r\n\
    if % = 1 goto roamreg\r\n\
  :regagain\r\n\
    if c > 120 goto regtimeout\r\n\
    let c=c+2\r\n\
    print \".\"\r\n\
    goto waitreg\r\n\
  :regtimeout\r\n\
    print \"\r\nFailed to register\r\n\"\r\n\
    exit 1\r\n\
  :homereg\r\n\
    print \"\r\nRegistered on Home network: \"\r\n\
    goto registered\r\n\
  :roamreg\r\n\
    print \"\r\nRegistered on Roaming network: \"\r\n\
    goto registered\r\n\
  :registered\r\n\
    waitquiet 1 0.1\r\n\
    send \"AT+COPS?^m\"\r\n\
    get 2 \"^m\" $s\r\n\
    get 2 \"^m\" $s\r\n\
    let a=len($s)\r\n\
    let b=a-12\r\n\
    if b < 1 goto regtimeout\r\n\
    let $c=$right($s,b)\r\n\
    print $c,\"\r\n\"\r\n";

char _3G2G_mode_code[] =
    "opengt\r\n\
    set com 115200n81\r\n\
    set senddelay 0.05\r\n\
    waitquiet 1 0.2\r\n\
    send \"AT_OPSYS=3^m\"\r\n\
    waitfor 10 \"OK\",\"ERR\"\r\n\
    if % = -1 goto timeout\r\n\
    if % = 0 goto cont\r\n\
    if % = 1 goto error\r\n\
  :timeout\r\n\
    print \"Timeout Error communicating with device.\r\n\"\r\n\
    exit 1\r\n\
  :error\r\n\
    print \"Error response from device.\r\n\"\r\n\
    exit 1\r\n\
  :cont\r\n\
    print \"Set 3G preferred mode\\r\n\"\r\n";


char _3G_mode_code[] =
    "opengt\r\n\
    set com 115200n81\r\n\
    set senddelay 0.05\r\n\
    waitquiet 1 0.2\r\n\
    send \"AT_OPSYS=1^m\"\r\n\
    waitfor 10 \"OK\",\"ERR\"\r\n\
    if % = -1 goto timeout\r\n\
    if % = 0 goto cont\r\n\
    if % = 1 goto error\r\n\
  :timeout\r\n\
    print \"Timeout Error communicating with device.\r\n\"\r\n\
    exit 1\r\n\
  :error\r\n\
    print \"Error response from device.\r\n\"\r\n\
    exit 1\r\n\
  :cont\r\n\
    print \"Set 3G only mode\\r\n\"\r\n";

char _2G_mode_code[] =
    "opengt\r\n\
    set com 115200n81\r\n\
    set senddelay 0.05\r\n\
    waitquiet 1 0.2\r\n\
    send \"AT_OPSYS=0^m\"\r\n\
    waitfor 10 \"OK\",\"ERR\"\r\n\
    if % = -1 goto timeout\r\n\
    if % = 0 goto cont\r\n\
    if % = 1 goto error\r\n\
  :timeout\r\n\
    print \"Timeout Error communicating with device.\r\n\"\r\n\
    exit 1\r\n\
  :error\r\n\
    print \"Error response from device.\r\n\"\r\n\
    exit 1\r\n\
  :cont\r\n\
    print \"Set 2G only mode\\r\n\"\r\n";

char _GTEDGE_code[] =
    "opengt\r\n\
    set com 57600n81\r\n\
    send \"AT+CFUN=1^m\"\r\n\
    waitquiet 5 0.2";

char _SETPIN_code[]=
    "opengt\r\n\
    set com 115200n81\r\n\
    set senddelay 0.05\r\n\
    waitquiet 1 0.2\r\n\
  :start\r\n\
    flash 0.1\r\n\
    send \"AT+CPIN?^m\"\r\n\
    waitfor 30 \"SIM PUK\",\"SIM PIN\",\"READY\",\"ERROR\",\"ERR\"\r\n\
    if % = -1 goto error\r\n\
    if % = 0 goto ready\r\n\
    if % = 1 goto getpin\r\n\
    if % = 2 goto ready\r\n\
    if % = 3 goto error\r\n\
    if % = 4 goto error\r\n\
  :error\r\n\
    print $s,\" ***SIM ERROR***\r\n\"\r\n\
    print \"Check device port configuration.\r\nCheck SIM is inserted\r\nTest SIM in a mobile phone?\r\n\"\r\n\
    exit 1\r\n\
  :getpin\r\n\
    #handle case where original Vodafone 3G generates wrong response\r\n\
    let $x=$env(\"COMGTPIN\")\r\n\
    let a=len($x)\r\n\
    if a=0 goto pinenverr\r\n\
    if a<>4 goto pinerror\r\n\
    let c=0\r\n\
  :test\r\n\
    let $c=$mid($x,c,1)\r\n\
    if $c<\"0\" goto pinerror\r\n\
    if $c>\"9\" goto pinerror\r\n\
    inc c\r\n\
    if c<4 goto test\r\n\
    let a=val($x)\r\n\
    if a<0 goto pinerror\r\n\
    if a>9999 goto pinerror\r\n\
    let $c=$left($x,4)\r\n\
  :enterpin\r\n\
    send \"AT+CPIN=\\\"\"\r\n\
    send $c\r\n\
    send \"\\\"^m\"\r\n\
    waitfor 20 \"OK\",\"ERR\"\r\n\
    if % = -1 goto timeerror\r\n\
    if % = 0 goto ready\r\n\
    if % = 1 goto pinerror\r\n\
  :pinenverr\r\n\
    print \"ERROR: The COMGTPIN env variable is not set\r\n\"\r\n\
    exit 1\r\n\
  :pinerror\r\n\
    print \"ERROR: PIN code must be 4 decimal digits only\r\n\"\r\n\
    print \"Caution! - entering the wrong PIN code three times will lock the SIM\r\n\"\r\n\
    exit 1\r\n\
  :timeerror\r\n\
    print \"ERROR: timeout, device did not respond to PIN command entry.\r\n\"\r\n\
    exit 1\r\n\
  :ready\r\n\
    print \"SIM ready\r\n\"\r\n";

char _SETAPN_code[]=
    "opengt\r\n\
    set com 115200n81\r\n\
    set senddelay 0.05\r\n\
    waitquiet 1 0.2\r\n\
  :start\r\n\
    flash 0.1\r\n\
  :getapn\r\n\
    let $x=$env(\"COMGTAPN\")\r\n\
    let a=len($x)\r\n\
    if a=0 goto apnerror\r\n\
    if a>32 goto apnerror\r\n\
  :enterapn\r\n\
    send \"AT+CGDCONT=1,\\\"IP\\\",\\\"\r\n\
    send $c\r\n\
    send \"\\\"^m\"\r\n\
    waitfor 20 \"OK\",\"ERR\"\r\n\
    if % = -1 goto timeerror\r\n\
    if % = 0 goto OK\r\n\
    if % = 1 goto apnerror\r\n\
  :apnerror\r\n\
    print \"ERROR entering APN\r\n\"\r\n\
    print \"The COMGTAPN env variable is not set\r\n\"\r\n\
    exit 1\r\n\
  :timeerror\r\n\
    print \"ERROR entering APN\r\n\"\r\n\
    print \"The device timeout.\r\n\"\r\n\
    exit 1\r\n\
  :OK\r\n\
    exit 0\r\n";


char _help_code[]  =\
    "print \"comgt Usage: \"\r\n\
    print \"comgt [options] [built in script]'|[external script]\r\n\r\n\"\r\n\
    print \"Built in scripts -\r\n\"\r\n\
    print \"  comgt [default]           Checks SIM status (requests PIN if required),\r\n\"\r\n\
    print \"                           registration and signal strength reported by\r\n\"\r\n\
    print \"                           datacard.\r\n\"\r\n\
    print \"  comgt info                Display configuration of datacard.\r\n\"\r\n\
    print \"  comgt sig                 Report Signal strength.\r\n\"\r\n\
    print \"  comgt reg                 Report Registration status.\r\n\"\r\n\
    print \"  comgt PIN                 Set PIN using COMGTPIN env variable.\r\n\"\r\n\
    print \"  comgt APN                 Set APN using COMGTAPN env variable.\r\n\"\r\n\
    print \"\r\n  Valid for GlobeTrotter GPRS only:\r\n\"\r\n\
    print \"      comgt USA                 Switch to 900/1900 MHz band for USA operation.\r\n\"\r\n\
    print \"      comgt Europe              Switch to 900/1800 MHz band for Europen operation.\r\n\"\r\n\
    print \"\r\n  Valid for GlobeTrotter EDGE and Combo EDGE only:\r\n\"\r\n\
    print \"      comgt GTEDGE             Switch on radio interface.\r\n\"\r\n\
    print \"\r\n  Valid for Vodafone 3G, GlobeTrotter Fusion, GT Max :\r\n\"\r\n\
    print \"      comgt 2G             2G networks only.\r\n\"\r\n\
    print \"      comgt 3G             3G networks only\r\n\"\r\n\
    print \"      comgt 3G2G           3G network preferred\r\n\"\r\n\
    print \"\r\nCommand line options (must be before script name) - \\r\n\"\r\n\
    print \"  -d device                Use alternative device. e.g -d /dev/ttyUSB0\r\n\"\r\n\
    print \"  -e                       Turn on communication echo.\r\n\"\r\n\
    print \"  -h                       Help.\r\n\"\r\n\
    print \"  -V                       Version.\r\n\"\r\n\
    print \"  -s                       Don't run internal 'default' script before running\r\n\"\r\n\
    print \"                           external script.\r\n\"\r\n\
    print \"  -t=\"\\r\n\"                    Set alternative line terminator (default=\"\\r\n\").\r\n\"\r\n\
    print \"  -v                       Verbose mode. Print lots of trace info.\r\n\"\r\n\
    print \"  -x                       Speed exchange. 115200 replaced by 57600.\r\n\"\r\n\
    print \"                           ***used for GlobeTrotter EDGE and Combo EDGE***\r\n\"\r\n\
    print \"NOTES:\r\ncomgt assumes that the GlobeTrotter device is /dev/modem (create a logical link\r\n\"\r\n\
    print \"to actual device or use -d switch). Unless you use the '-s' switch comgt will\r\n\"\r\n\
    print \"run the internal 'default' script first before running an external script file.\r\n\"\r\n" ;




char *get_code(char* name){
  if(strlen(name)==0) return(_default_code);
  if (strcmp(name,"default")==0) return (_default_code);
  if (strcmp(name,"help")==0) return (_help_code);
  if (strcmp(name,"info")==0) return (_info_code);
  if (strcmp(name,"USA")==0) return (_USA_code);
  if (strcmp(name,"Europe")==0) return (_Europe_code);
  if (strcmp(name,"sig")==0) return (_sig_code);
  if (strcmp(name,"reg")==0) return (_reg_code);
  if (strcmp(name,"GTEDGE")==0) return (_GTEDGE_code);
  if (strcmp(name,"2G")==0) return (_2G_mode_code);
  if (strcmp(name,"3G")==0) return (_3G_mode_code);
  if (strcmp(name,"3G2G")==0) return (_3G2G_mode_code);
  if (strcmp(name,"PIN")==0) return (_SETPIN_code);
  if (strcmp(name,"APN")==0) return (_SETAPN_code);
  return(NULL);
}

