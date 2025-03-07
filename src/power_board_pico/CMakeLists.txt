if(BUILD_FOR_PICO)

  pico_sdk_init()

  add_executable(power_board_pico
    main.cpp
  )
  add_subdirectory(keystone)
  target_include_directories(power_board_pico PRIVATE
    "./"
  )

  target_compile_definitions(power_board_pico PRIVATE
    PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE=1
    GIRAFFE_VERSION_NUMBER="${GIRAFFE_VERSION_NUMBER}"
    GIRAFFE_VERSION_STAGE="${GIRAFFE_VERSION_STAGE}"
  )

  target_link_libraries(power_board_pico PUBLIC
    # Internal
    keystone

    # pico libraries
    pico_stdlib
    hardware_adc
    hardware_exception
    hardware_i2c

    # third party
    FatFs_SPI
  )

  # Using USB is annoying when uploading as the serial port resets.
  # unset(USE_USB_FOR_UART_STDIO CACHE)
  set(USE_USB_FOR_UART_STDIO OFF CACHE BOOL "Use USB for UART stdio")

  if(USE_USB_FOR_UART_STDIO)
    message(STATUS ">PICO: Using UART and USB")
    pico_enable_stdio_usb(power_board_pico 1) # Enable USB serial output.
    pico_enable_stdio_uart(power_board_pico 1) # Disable UART output.
    target_compile_definitions(power_board_pico PRIVATE
      PICO_STDIO_USE_USB=1
    )
  else()
    message(STATUS ">PICO: Using UART (Not USB)")
    pico_enable_stdio_usb(power_board_pico 0) # Disable USB serial output.
    pico_enable_stdio_uart(power_board_pico 1) # Enable UART output.
    target_compile_definitions(power_board_pico PRIVATE
      PICO_STDIO_USE_USB=0
    )
  endif()
  # pico_enable_stdio_usb(power_board_pico 1) # Enable USB serial output.
  # pico_enable_stdio_uart(power_board_pico 0) # Disable UART output.

  # Create map/bin/hex/uf2 file in addition to elf.
  pico_add_extra_outputs(power_board_pico)
endif()
