// HEADER TO MANAGE THE DEBUGGER FLAGS

#ifndef _DEBUG_H
#define _DEBUG_H

  // #define SERIAL_BAUD 57600             // 9600, 19200, 38400, 57600, 74880, 115200, 230400, 250000, 500000, 1000000, 2000000
  #define DEBUG_BUFFER_SIZE   256
  
  // WARNING: PLEASE BE CAREFUL HERE!!! This buffer must be large enough to handle a fully formatted debug string, which includes all the system values...
  //    DONT MAKE IT TOO BIG AS MEMORY IS TIGHT...
  extern char debug_stringBuf[DEBUG_BUFFER_SIZE];     // One char string to construct/hold all messages (debug, trace, error and info)
  
  #define keystatus(fmt, ...)               do { sprintf(debug_stringBuf,"KEYSTATUS - %d:%s(): " fmt, __LINE__, __func__, ##__VA_ARGS__); Serial.println(debug_stringBuf); } while(0)
      
  // Set the following defines at before #include "Debug.h" to turn on/off level of debugging...
  //  #define DEBUG                           TRUE
  //  #define DEBUG_HIFREQ                    TRUE
  //  #define TRACE                           TRUE
  //  #define TRACE_HIFREQ                    TRUE
  //  #define INFO                            TRUE
  //  #define ERROR                           TRUE
  //  #define WARNING                         TRUE

  #ifdef ERROR
    // #define error(fmt, ...)               do { sprintf(debug_stringBuf,"ERROR - %s:%d:%s():" fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__); Serial.println(debug_stringBuf); } while(0)
    #define error(fmt, ...)               do { sprintf(debug_stringBuf,"ERROR - %d:%s():" fmt, __LINE__, __func__, ##__VA_ARGS__); Serial.println(debug_stringBuf); } while(0)
  #else
    #define error(fmt, ...)
  #endif

  #ifdef WARNING 
    // #define warning(fmt, ...)             do { sprintf(debug_stringBuf,"WARNING - %s:%d:%s():" fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__); Serial.println(debug_stringBuf); } while(0)
    #define warning(fmt, ...)             do { sprintf(debug_stringBuf,"WARNING - %d:%s():" fmt, __LINE__, __func__, ##__VA_ARGS__); Serial.println(debug_stringBuf); } while(0)
  #else
    #define warning(fmt, ...)
  #endif

  #ifdef INFO
    // #define info(fmt, ...)                do { sprintf(debug_stringBuf,"INFO - %s:%d:%s():" fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__); Serial.println(debug_stringBuf); } while(0)
    #define info(fmt, ...)                do { sprintf(debug_stringBuf,"INFO - %d:%s():" fmt, __LINE__, __func__, ##__VA_ARGS__); Serial.println(debug_stringBuf); } while(0)
  #else
    #define info(fmt, ...)
  #endif
  
  #define debug(fmt, ...)             
  #define debug_nolf(fmt, ...) 
  #define debug_hifreq(fmt, ...)             
  #define debug_hifreq_nolf(fmt, ...) 
  #define trace(fmt, ...)             
  #define trace_nolf(fmt, ...) 
  #define trace_hifreq(fmt, ...)             
  #define trace_hifreq_nolf(fmt, ...) 
      
  #ifdef DEBUG
    #undef debug
    #undef debug_nolf
    
    // #define debug(fmt, ...)                     do { sprintf(debug_stringBuf,"DEBUG - %s:%d:%s():" fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__); Serial.println(debug_stringBuf); } while(0)
    #define debug(fmt, ...)                     do { sprintf(debug_stringBuf,"DEBUG - %d:%s():" fmt, __LINE__, __func__, ##__VA_ARGS__); Serial.println(debug_stringBuf); } while(0)
    #define debug_nolf(fmt, ...)                do { sprintf(debug_stringBuf,fmt, ##__VA_ARGS__); Serial.print(debug_stringBuf); } while(0)
  
  // Hi-Frequency Debug - Will severly impact on performance
    #ifdef DEBUG_HIFREQ
      #undef debug_hifreq
      #undef debug_hifreq_nolf
      
      #define debug_hifreq(fmt, ...)            do { sprintf(debug_stringBuf,"DEBUG-HIFREQ - " fmt, ##__VA_ARGS__); Serial.println(debug_stringBuf); } while(0)
      #define debug_hifreq_nolf(fmt, ...)       do { sprintf(debug_stringBuf,fmt, ##__VA_ARGS__); Serial.print(debug_stringBuf); } while(0)
    #endif
  #endif

  #ifdef TRACE
    #undef trace
    #undef trace_nolf
    
    // #define trace(fmt, ...)                     do { sprintf(debug_stringBuf,"TRACE - %s:%d:%s():" fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__); Serial.println(debug_stringBuf); } while(0)
    #define trace(fmt, ...)                     do { sprintf(debug_stringBuf,"TRACE - %d:%s():" fmt, __LINE__, __func__, ##__VA_ARGS__); Serial.println(debug_stringBuf); } while(0)
    #define trace_nolf(fmt, ...)                do { sprintf(debug_stringBuf,fmt, ##__VA_ARGS__); Serial.print(debug_stringBuf); } while(0)
    
    // Hi Frequency Tracing and Debug - Will severly impact on performance
    #ifdef TRACE_HIFREQ
      #undef trace_hifreq
      #undef trace_hifreq_nolf
       
      #define trace_hifreq(fmt, ...)            do { sprintf(debug_stringBuf,"TRACE-HIFREQ - " fmt, ##__VA_ARGS__); Serial.println(debug_stringBuf); } while(0)
      #define trace_hifreq_nolf(fmt, ...)       do { sprintf(debug_stringBuf,fmt, ##__VA_ARGS__); Serial.print(debug_stringBuf); } while(0)
    #endif
  #endif
#endif
