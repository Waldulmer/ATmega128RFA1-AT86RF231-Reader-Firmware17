/* Copyright (c) 2008  ATMEL Corporation
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, data, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/
/**
   @mainpage Pinpoint Systems Network (Route-Under-MAC)

   @section Overview

   The Pinpoint OnlineSystem enables route operators to provide customers cashless transactions 
   through the use of an Online Account. A user account is where the customer has deposited 
   a small amount of money with the operator. The account is debited and transactions take place 
   when users activate washers or dryers. Adding money to an account is accomplished via WEB interface. 
   The Pinpoint OnlineSystem constitutes two major building blocks - the Pinpoint OnlineSystems network 
   and the Pinpoint WEB Application. The Pinpoint Network is supported by the Raspberry Pi and 
   the ATmega128RFA1 microcontroller. 

   @section more_info More Information Available

   Atmel appnote <b>AVR2070</b> includes much of the info in this
   Doxygen-generated documentation, and also guides you through
   getting this software working on a variety of hardware platforms.
   The appnote is available on Atmel's website
   http://www.atmel.com/wireless.  This document gives details about
   the inner workings of the firmware, while the AppNote is oriented
   toward helping a new user get the firmware running on real
   hardware.

   @section intro Introduction

   The RUM network is a simple, small, multi-hop, self-forming
   network.  It has the following characteristics:

   - Small object size.  Current implementation (MAC with tiny app) is
     well under 8KB for an end node.

   - Coordinator is required.  The coordinator is the only node that has
     any "smarts", and the routers and end-nodes are quite simple.
     This allows for cheap hardware for both routers and end-nodes.

   - Self-forming network.  The nodes wake up and can find a network
     and associate to it.

   - Multi-hop routing is supported.  There is no practical limit to
     the number of hops.

   - IPv6 Support allows nodes to communicate across the world seamlessly.

   - Open protocol.  Free for use with Atmel hardware.

   @section api Application Programming Interface

   An application talks to the MAC by using a few function calls in
   the MAC, and the MAC communicates events back to the application by
   calling pre-defined callback functions.  The relevant MAC functions
   and the callback functions are declared in the header file @ref
   system.h.

   All of the application-level RUM API callback functions exist in
   application.c.  This file should be used as a basis for new
   applications.  Particularly, the appAssociateConfirm() function is
   called when the node becomes associated to a network.  Users should
   should use this function as a starting point for
   application-specific behavior.

   As shipped, the firmware will automatically initialize the network
   and create connections between nodes.  A user application does not
   need to do anything to create the network.  The three basic
   functions to be used by an application are:

   - appAssociateConfirm() to begin operating on the network.
  
   - macDataRequest() to send data.

   - appDataIndication() to process received data.

   The functions listed below are the major parts of the MAC software,
   but are not intended to be used directly by the application.

   @subsection coord_commands Coordinator commands

   - macFindClearChannel() is called on startup.  The new coordinator
     node finds a clear channel by doing an energy scan and finding
     the quietest channel.  Alternatively, a pre-defined channel can
     be set in @ref PAN_CHANNEL.

   - appClearChanFound() is called when the scan is complete.  The
     coordinator chooses a channel and PAN ID by calling
     macStartCoord(), and is then ready for operation.

   - When the coordinator receives a beacon request, it calls
     sendBeaconFrame(), which sends a beacon back to the requester.

   - When the coordinator receives an association request, it calls
     macAssociationResponse(), which stores the new node's information
     in the coordinator's network table, and issues an association
     response frame.

   @subsection router_commands Router and end node commands

   On the router/end node side, several functions are called in
   sequence associate to the network.  The timing of these calls are
   regulated by the MAC's @ref timer_module, by making calls to
   macSetAlarm().

   - macInit() is called to initialize the radio and the MAC.

   - macScan() is then called to search for a network to join.  This
     causes the node to send out beacon request nodes on every
     channel, and to record the beacons it gets back.  The best node
     is chosen.

   - appScanConfirm() is called when the scan is complete.  If the
     scan was successful, then appScanConfirm calls appAssociate,
     which in turn calls ...

   - macAssociate() - this sends an association request to the
     coordinator (sometimes via intermediate nodes), and processes an
     association response packet with ...

   - appAssociateConfirm() is called when the node either associates
     successfully, or times out waiting to associate.

   After the nodes have associated the macConfig.associated flag is
   set to true and, all nodes communicate using the same functions:

   - macDataRequest() is called by the sending node, or macPing() is
     called to ping another node.  The MAC calls back to either
     appPacketSendSucceed() or appPacketSendFailed().

   - macDataIndication() is called by the MAC if a packet is received
     that is addressed to this node.   

   There are a few other useful functions that the MAC offers.  These
   functions are useful for making a non-networking applications, such
   as a remote control unit.

   - macIsChild() reports on whether a given node is a child of this
     node.

   - macSetOperatingChannel() can be used to manually set the radio
     channel.

   - radioGetPartnum() will query the Atmel radio chip and return the
     part number.

   - radioGetSavedRssiValue() returns the last measured received
     signal strength indication (RSSI) value for a received packet.

   - radioGetSavedLqiValue() returns the last measured link quality
     indication (LQI) value for a received packet.

   - radioGetOperatingChannel()

   - radioGetTxPowerLevel() and radioSetTxPowerLevel() set and read
     the output RF power levels

   - radioBatmonGetVoltageThreshold(), radioBatmonGetVoltageRange(),
     radioBatmonConfigure(), and radioBatmonGetStatus() are used to
     work with the RF2xx on-board battery monitor function.

   - radioGetClockSpeed() and radioSetClockSpeed() allow the use of
     the RF2xx CLKM signal, which can be used to clock the CPU or
     provide an accurate timebase to calibrate any other oscillator.

   - radioEnterSleepMode() puts the radio chip to sleep and
     radioLeaveSleepMode() wakes up the radio chip.

   - radioSendData() sends a "raw" packet over the radio.  This is a
     lower-level function that RUM uses to send data to another node.

   - radioRandom() returns up to 8 bits of random data, created from
     the random radio noise on the RF2xx chip.  The RF230 does not
     have a random number generator, so the radioRandom function only
     returns a random number from the rand() system function.

   - nodeSleep() Puts the entire node to sleep for a specified time.

   Other MAC parameters reside in the @ref macConfig structure.  While
   this structure is not meant to be used by the application directly,
   several useful parameters are available for reference:

   - @b longAddr - The long (MAC) address of this node
   - @b associated - True if this node has been associated to a network
   - @b panId - The PAN ID of this node
   - @b shortAddress - the short address of this node
   - @b parentShortAddress - the short address of this node's parent
   - @b currentChannel - the current radio channel selected

   The @ref timer_module can be used by an application to execute
   functions after a non-blocking delay.

   The @ref serial module provides a serial port for the AVR targets.     

   @section comps Components

   There are a few modules that make up this project.  These are all
   compiled together into one object file, but some of these modules
   can be left out of the compilation based on @ref compile_options.

   - @ref app provided as a basis for a simple user application.   

   - @ref mac.  A subset of the MAC defined by IEEE 802.15.4.  This
     includes multi-hop and self-forming functionality.

   - @ref radio between the MAC layer to the Atmel AT86RF2xx radio chip.

   - @ref timer_module to support MAC events and any user-created events.

   - @ref serial code to support debugging or a serial connection to
     another device.  This code can be left out of the project to
     reduce object code size.

   - @ref pposreader to demonstrate a data-collection network.
     

   @section esplanade How the components work together

   The example application runs on "bare metal", that is, without any
   kind of operating system.  In main(), there is a "forever" loop
   that continously calls the two program "threads", appTask() and
   macTask().  These tasks process the events that occur in the
   background, such as the timer interrupt and the radio interrupt.

   The interrupt routines that handle radio and timer events post an
   event object into the event queue (see @ref mac_put_event()).  The
   macTask() function checks this queue and processes posted events,
   most of which were initiated by an interrupt service routine (ISR).

   The @ref serial runs by itself in a stand-alone way.  The only
   interaction between the serial module and the rest of the program
   is through functions like serial_getchar() and serial_putchar().  A
   serial queue is used for both sending and receiving characters over
   the serial port.

   The @ref timer_module implements an easy-to-use interface to allow
   delayed processing of events.  This module is available for use by
   the

   The @ref radio provides a device driver for the radio chip.
   This layer resides below the MAC layer, and the two layers talk
   directly to each other.

   @section compiling Compiling

   This package can be compiled on several host platforms, with many
   options for the target environment.  The code is written for the
   GCC compiler (ver 4.9.2), which has been compiled as a cross
   compiler for AVR.  See the avr-libc project
   http://www.nongnu.org/avr-libc/ for details.

   @subsection hostwin Windows Host Platform

   Compile and program the code into the target using Atmel's AVR
   Studio (available free at http://www.atmel.com).  You will also
   have to install WinAVR, which is the AVR GCC toolchain compiled for
   a Win32 host.

   There are AVR Studio project files supplied for the various builds.
   Load the project file and press the compile button.  If you have a
   debugger connected to the target, you may load the code into the
   target and begin debugging.   

   @subsection compile_options Compile-time options

   These options affect which features are included in a RUM build.
   These options are in the Makefile or can be added to the AVR Studio
   project file.

   - TYPE can be END, ROUTER, or COORD.  This directs which kind of
     node is being compiled.  This parameter is required.  The
     makefile sets the @ref NODETYPE macro. In mac.h the Nodetype is set to ROUTERNODE.

   - @ref PLATFORM is set to RCBRFA1.  
     This sets the MCU flag and other parameters to compile the code for a specific platform.  
	 If this parameter is left out, the PLATFORM will be set to the default (COORD).  
	 Note that a platform defines how the CPU and radio chip are connected
     together. It does not specify which radio chip is being used,
     though it does specify the @ref BAND macro.

   - @ref IPV6LOWPAN is set to 0.  
     If IPV6LOWPAN is zero, then no 6LoWPAN or IPv6 code will be compiled
     in. Disabling IPV6 saves some space if it will not be used.

   - @ref DEBUG is set to 0.  This enables/disables diagnostic
     messages from the serial port.  Disabling DEBUG will greatly
     reduce then object code size (flash size).  Note that either @ref
     SERIAL or @ref OTA_DEBUG must be set to use the DEBUG option.

   - @ref DEMO is set to 0.  In DEMO mode, a node picks its parent node
     as the one with the highest RSSI.  Otherwise, a parent is picked
     by (in sequence) best LQI, lowest number of hops to the
     coordinator, and then RSSI.

   - @ref APP is set to 0.  If APP is zero, then no
     application is linked in other than the default minimal
     application.

   - @ref RUMSLEEP is set to 0. This enables/disables the sleep
     code.  In the sensor application, an end node will go to sleep
     between readings if RUMSLEEP is set to one.  This sleep code is
     general-purpose and can be used by new applications also.  On
     coordinator and router nodes, this option makes a parent node
     store a frame destined for a sleeping child node, and will send
     the frame to the child immediately after it has woken up.  See
     also the @ref VLP flag.

   - @ref PAN_CHANNEL is not set. The node will scan all channels.
     This applies to coordinators, routers, and end nodes.  Setting
     PAN_CHANNEL on a coordinator fixes the network to that channel.
     Setting PAN_CHANNEL on a router or end node means that the node
     will only scan one channel, and cannot find any network operating
     on any other channel.

   - @ref PAN_ID sets the PAN ID for the node.  Only the coordinator
     uses this definition.  If PAN_ID is not set, then the coordinator
     chooses a random PAN ID.  The routers and end nodes get the PAN
     ID from the coordinator.

   - @ref WDOG_SLEEP sets the timing source for sleeping to be the
     on-board AVR watchdog timer.  If WDOG_SLEEP is set to zero, then
     the low-power oscillator should be connected to a 32.768KHz
     crystal.

   After compiling, the code is loaded using a JTAGICE.    

   @section other_flags Other compilation options

   There are a few other compilation options that are not global.
   These may be defined in the AVR Studio project settings, or on the
   avr-gcc command line.  They may also be changed in the source code
   to permanently set the option.

   - @ref SENSOR_TYPE configures which type of sensor to read.  If
     SENSOR_TYPE is not set, the node defaults to sending random data.
     This option is set in @ref sensors.c, where the valid sensor
     types are listed.   

   - @ref CAL allows the sensor calibration code to be removed from
     the build.  This option is not used.

   - @ref VLP .  This option is off by default.

   - @ref SERIAL brings in the serial code.  If you are compiling with
     @ref DEBUG on, then you need either SERIAL or OTA_DEBUG set.

   - @ref OTA_DEBUG allows sending debug messages over the air, to be
     displayed by the coordinator.  This option is useful for getting
     debug information from nodes that do not have a serial port.

   @section troubleshooting Troubleshooting RUM

   There are a several steps to bringing up RUM on a new platform (or
   even a new board of an existing platform).  This section guides you
   through the steps to verify functionality of the platform.

   @b NOTE: These instructions apply to the AVR-based platforms.  There
   may be slight differences for troubleshooting other platforms,
   although the goal of each step is the same.

   -# Verify that the radio is communicating with the microcontroller.
      On startup, the microcontroller intializes the radio chip using
      the SPI port.  Part of that initialization involves setting a
      radio register and then waiting for that register setting to be
      read back correctly.  If the commuication is not working, then
      the code will hang at this point.  The simplest way to verify
      that the radio is properly communicating is to set a breakpoint
      on the macTask() function, and run the program.  If the
      breakpoint is reached, then the radio is properly communicating
      over the SPI port.
   -# Verify that the system timer is running.  Set a breakpoint on
      the Interrupt Service Routine, which is in avr_timer.c If this
      breakpoint is hit, then the system timer is working.
   -# Verify that the radio is able to generate system interrupts.
      Set a breakpoint on the radio ISR in hal_avr.c (look for
      "ISR(RADIO_VECT)").  If this breakpoint is reached, then the
      radio is generating interrupt signals, and the microcontroller
      is servicing them.  The radio should generate interrupts on
      startup, because the radio sends beacon request packets as part
      of a scan, and the radio generates an interrupt each time it
      sends a packet.
   -# Verify that the radio is receiving packets.  You can trace the
      parsing of an incoming packet by putting breakpoints inside the
      following functions:
      - radioTrxEndEvent() - put a breakpoint on the line "hal_frame_read();"
      - macTask() - look at cases MAC_EVENT_SCAN and MAC_EVENT_RX.
      - macDataIndication() - trace through this function so see that
        the packet is properly dispatched.
 */

/*
        @defgroup mac_event MAC event handler
        @defgroup mac_route MAC routing functions
        @defgroup mac_scan MAC channel scanning functions
*/


#ifndef SYSX_H
#define SYSX_H

// Definitions of data structures used.
// Definitions of called and "callback" function names in the entire application.
// These functions must be defined in each layer - even if the function is empty.

#include "mac.h"

// Define some applications
/**
   Macro to define the sensor application. See @ref APP.

   @ingroup app
*/
#define SENSOR      1
#define IPSO        2
#define DSKIPDEMO   3
#define TRACKER     4
#define VOICE       5     // Only used with AVR32
#ifndef APP
/**
   The APP macro is used to define which application gets compiled in.
   If this macro is defined as zero, then only the basic demo
   application gets compiled.  If you set this macro to the value of
   @ref SENSOR, then the sensor app gets compiled in.

   @ingroup app
 */
#define APP SENSOR
#endif



/**
   Enable or disable the sleep function by setting this macro to 1
   (on) or 0 (off).

   @ingroup app
*/
#ifndef RUMSLEEP
#define RUMSLEEP 1
#endif

/**
   Enable or disable the IPV6/6LoWPAN layer by setting this macro to 1
   (on) or 0 (off).

   @ingroup app
*/
#ifndef IPV6LOWPAN
/*  #if (__AVR__ || __AVR32__) && NODETYPE == COORD */
/*   #define IPV6LOWPAN 0      // No AVR 6LoWPAN coordinators allowed */
/*   #warning "IPV6LOWPAN set to zero for AVR target coordinator" */
/*  #else */
  #define IPV6LOWPAN 1
/*  #endif */
#endif

/**
   Define which IPv6 stack we are using: RUM's minimal stack, or
   uIPv6's full featured stack.

   @ingroup app
*/
#define STACK_NONE  0
#define STACK_RUM   1
#define STACK_UIP   2

#if IPV6LOWPAN == 0
#define IPV6_STACK STACK_NONE
#endif

#ifndef IPV6_STACK
#if NODETYPE == COORD
#define IPV6_STACK STACK_UIP
#else
#define IPV6_STACK STACK_RUM
#endif
#endif

// App functions
void appInit(void);

// App layer callbacks
void appScanConfirm(u8 success);
void appClearChanFound(u8 channel);
void appAssociateConfirm(u8 success);
void appPacketSendAccessFail(void);
void appPacketSendFailed(void);
void appPacketSendSucceed(void);
void appTask(void);
void appPingReq(u16 addr);
void appPingRsp(u16 addr);
void appSixlowpanPingResponse(void);
void appSixlowpanUdpResponse(char *str);
void appDataIndication(void);
void appChildAssociated(u16 shortAddress);
void appNodeAssociated(u16 shortAddress);
void appSixlowpanUdpResponse(char *str);
void appBeaconReceived(void);

// MAC layer functions
void macInit(u8 Channel);
void macStartCoord(void);
void macScan(void);
u8 macIsScanning(void);
void macAssociate(u16 shortAddr, u8 channel);
void macDataRequest(u16 addr, u8 len, u8 * data);
void macPing(u8 pingType, u16 addr);

// Radio/HAL callback functions (called from driver)
void radioTrxEndEvent(void);

#endif
