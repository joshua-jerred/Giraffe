if(BUILD_FOR_PICO)

  pico_sdk_init()

  add_executable(power_board_pico
    main.cpp
  )
  
  target_compile_definitions(power_board_pico PRIVATE
    PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE=1
    GIRAFFE_VERSION_NUMBER="${GIRAFFE_VERSION_NUMBER}"
    GIRAFFE_VERSION_STAGE="${GIRAFFE_VERSION_STAGE}"
  )

  target_link_libraries(power_board_pico 
    pico_stdlib
    hardware_adc
    hardware_exception
  ) # Link the pico standard library.
  
  # enable usb output, disable uart output
  pico_enable_stdio_usb(power_board_pico 1) # Enable USB serial output.
  pico_enable_stdio_uart(power_board_pico 0) # Disable UART output.

  # Create map/bin/hex/uf2 file in addition to elf.
  pico_add_extra_outputs(power_board_pico)
endif()
