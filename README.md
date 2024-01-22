# USB-Mon / AVR Flash SW

Ever liked to monitor a PC or Home Server with a small LCD display?

USB-Mon does it on a simple way.

## In a Nutshell

USB-Mon intents to be a light health monitor for home-brewed server or PCs which run without any display.<br/>
The central controller is an ATmega8A which:

1. Receives commands from serial port (I use a USB to serial converter in front of ATmega's serial port)
2. Controls a LCD display attached on an 8-bit output port
3. Computes a consolidated overall state of all pages enabled
4. Starts blinking if the overall state is different than "okay"
5. Has a nightmode to disable blinking at nights
6. Has got a page/state timeout which warns about absent "okay" signals
6. Supports external push buttons to trigger background light and page flip

This repository holds the software to run both, the UART listener as well as the display control. 

## Service/Page states
Each page allos to set a name for a service or state which is controlled. <br/>
The states being allowed are:
- 0 = undefined (default, no timeout)
- 1 = okay
- 2 = unknown (being set after service ran in timeout)
- 3 = warning (persisent until a new "okay" is sent)
- 4 = error (persistent until a new "okay" is sent)

## UART Commands

Each command uses the same syntax, which is: <br/>
**@:\<command-char>\<command-data>**

### Command List

|Command Name|Call Syntax|Description|Example|
| --- | --- | --- | --- |
|**Debug Mode**| **@:d** | Toggles the debug mode. By default this mode is disabled. Sending the command once enables debug. Sending a second time disables it| n/a |
|**Page Count**| **@:p\<xx>**| Depending on how many services or parameters you like to monitor you may set the number of pages to be displayed.<br/><br/>The value xx needs to be sent as hex value.<br/><br/>The minimum is 0x01 (1), the maximum number of pages is 0x14 (20). Sending a 0x00 (0) will reset the default / maximum. | Enable 5 pages to be displayed: <br/><br/> @:p05|
|**Page Time**| **@:t\<xx>**| Define the time between a page flip in seconds. The default value is about 2sec. You may send a value between 0x00 (0), which resets to default and 0xFF (255). <br/><br/>Please note, this time in seconds is only an estimation and may vary due to the real spead of the ATmega. | Set a time of ten seconds for each page to show:<br/><br/> @:t0A |
|**Display Mode**| **@m\<xx>** | Defines the display mode to use. Available values are: <br/> 0x00 .. default mode <br/> 0x01 .. night mode, with blinking disabled. | Enable night mode:<br/><br/> @:m01 |
|**Service Name**| **@n\<xx>( \<y> \| \\\<yy> )+!** | Defines a name for a certain page, where \<xx> defines the page number to set the name for. This can have a range from 0x00 to 0x13. The page number is followed by a name string, which needs to be terminated using '!'. The name string consists of either single byte characters like 'a' or quoted hex numbers like \\df, which identifies ascii character number 223. <br/><br/> Note: The maximum length accepted is 14 characters. More are skipped. | Examples: <br/><br/> @:n01Hello World!<br/>@:n02Temp in \\dfC!|
|**Service State (Simple)**|**@:s\<xx>\<yy>**|Sets the service state as defined above with the given number.<br/>\<xx> .. defines the page<br/>\<yy> .. sets the state with range 0x00 (undefined) to 0x04 (error)| Set okay for service 01:<br/><br/>@:s0101|
|**Service State (Custom)**|**@:c\<xx>\<yy>\<state>**|Set the service state as defined above with the given number.<br/>\<xx> .. defines the page<br/>\<yy> .. sets the state with range 0x00 (undefined) to 0x04 (error) and <br/><state> .. alows to set a string as done with "Service Name" but with a maximum length of 5 characters.|Set service state for page 3 to warning and status text "oops": <br/><br/>@:c0203oops!<br/><br/>Note: Page numbers start with 0|
|**State Expiry**|**@:e\<xxxx>|Define the global expiry for all services, which are set to "okay". As soon as time xxxx exceeded since last update of a certain page, the service gets invalidated and is shown as state unknown. The display expects to get an updated state within expiry time which is expressed with a 2-byte number in seconds.|Set the page/state expiry to one hour (3600sec):<br/><br/>@:e0e10

## ATmega8A Default Pinning
Although the pinning can be easily change by modifying ***#define*** statements in global.h there is a default, which is described here:

... to be done.

# Licensing
In case you intend any use of this piece of software please read and accept the license agreement in file /LICENSE.