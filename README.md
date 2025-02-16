## Project Overview

This project is an implementation of an ESP32-based Android TV controller using the Android TV Remote protocol v2. It leverages the PlatformIO ecosystem for development and includes various features and functionalities to control and monitor the Android TV. This implementation does not require ADB or enabling developer tools on the Android TV device. It only requires the [Android TV Remote Service](https://play.google.com/store/apps/details?id=com.google.android.tv.remote.service) that comes pre-installed on most Android TV devices.

Key list you can see at [RemoteKeycode.h](./include/remote/RemoteKeycode.h)

### TODO
- [X] Pairing
- [X] Send command
- [ ] Add voice support

### Credits
- Official [implementation](https://android.googlesource.com/platform/external/google-tv-pairing-protocol/+/refs/heads/master) of the pairing protocol in Java
- [Implementation](https://github.com/farshid616/Android-TV-Remote-Controller-Python) in Python but for the old v1 protocol
- [Implementation](https://github.com/louis49/androidtv-remote) in Node JS for the v2 protocol
- [Implementation](https://github.com/tronikos/androidtvremote2?tab=readme-ov-file) in python for the v2 protocol
- [Description](https://github.com/Aymkdn/assistant-freebox-cloud/wiki/Google-TV-(aka-Android-TV)-Remote-Control-(v2)) of the v2 protocol

### Prerequisites

- [PlatformIO](https://platformio.org/) installed on your development environment.
- ESP32 development board.
- Necessary libraries and dependencies (specified in `platformio.ini`).

### Installation

1. Clone the repository:
    ```sh
    git clone https://github.com/nguyluky/ESP32_android_tv_remote_v2.git
    ```
2. Navigate to the project directory:
    ```sh
    cd ESP32_android_tv_remote_v2
    ```
3. Open the project in your preferred IDE (e.g., VS Code with PlatformIO extension).

### Building and Uploading

1. Connect your ESP32 board to the computer.
2. Build the project:
    ```sh
    pio run
    ```
3. Upload the firmware to the board:
    ```sh
    pio run --target upload
    ```

### Usage

- Power on the ESP32 board.
- Use the remote control to operate the Android TV.

## Contributing

Contributions are welcome! Please fork the repository and create a pull request with your changes.