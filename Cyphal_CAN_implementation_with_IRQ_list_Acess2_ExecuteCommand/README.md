# Cyphal/CAN Node on STM32

This project is an implementation of a Cyphal/CAN node on an STM32 microcontroller. The firmware is designed to demonstrate core Cyphal features, including node heartbeating, remote register access, and command execution.

The node is configured with a default ID of 96. It defines several registers, such as "my_register_2nd," which can be read and written remotely.

***

## Hardware Requirements

* **STM32 Microcontroller**: An STM32 board (e.g., Nucleo, Discovery) to run the firmware.
* **CAN Transceiver**: An external CAN transceiver module (e.g., TJA1050, MCP2551) is required to interface the microcontroller's CAN peripheral with the physical CAN bus.

***

## Building and Flashing

1.  **Open the Project**: Open the source code in your preferred STM32 development environment (e.g., STM32CubeIDE).
2.  **Build**: Compile the project to generate the binary file.
3.  **Flash**: Flash the generated binary onto your STM32 board using a debugger/programmer (like ST-Link).

***

## Host Machine Setup

Before interacting with the node, you must configure your host machine (e.g., a Linux PC with a CAN interface).

### Step 1: Configure CAN Interface

```bash
sudo ip link set can0 up txqueuelen 65535 type can bitrate 1000000
```

### Step 2: Install Yakut

Yakut is a command-line tool for interacting with Cyphal networks.

```bash
pip install yakut
pip install yakut[joystick]  # Optional, for joystick support
```

### Step 3: Set Up DSDL Namespace

```bash
mkdir -p ~/.cyphal
wget https://github.com/OpenCyphal/public_regulated_data_types/archive/refs/heads/master.zip -O dsdl.zip
unzip dsdl.zip -d ~/.cyphal
cp -r ~/.cyphal/public_regulated_data_types-master/* ~/.cyphal/
rm -rf ~/.cyphal/public_regulated_data_types-master dsdl.zip
```

### Step 4: Configure Environment Variables

```bash
export UAVCAN__CAN__IFACE="socketcan:can0"
export UAVCAN__CAN__MTU=8
export UAVCAN__NODE__ID=96
export CYPHAL_PATH="$HOME/.cyphal"
```

Make these settings persistent by adding them to your `~/.bashrc`:

```bash
echo 'export UAVCAN__CAN__IFACE="socketcan:can0"' >> ~/.bashrc
echo 'export UAVCAN__CAN__MTU=8' >> ~/.bashrc
echo 'export UAVCAN__NODE__ID=96' >> ~/.bashrc
echo 'export CYPHAL_PATH="$HOME/.cyphal:$CYPHAL_PATH"' >> ~/.bashrc
source ~/.bashrc
```

***

## Interacting with the Node

### Monitoring the Network

```bash
yakut monitor
```

### Subscribe to Heartbeat

```bash
yakut sub uavcan.node.Heartbeat.1.0
yakut sub uavcan.node.Heartbeat.1.0 --with-metadata
```


### Publishing Messages for Testing

```bash
yakut pub 1620:uavcan.primitive.array.Real64.1.0 '{value: [1.1, 2.2, 3.3]}'
```

### Working with Registers

List all available registers:

```bash
y rl 96
```

Read a register value:

```bash
yakut register-access 96 "my_register_2nd"
```

Write a register value:

```bash
y r 96 my_register_2nd 100
```

### Executing Commands

Store persistent states:

```bash
yakut execute-command 96 store
```


Restart the node:

```bash
y cmd 96 restart
```


Factory reset (all register value=0):

```bash
y cmd 96 factory_reset
```

## 🔒 Persistent Register Storage in Flash (STM32F446RE)

This project stores **Cyphal register values** persistently in internal Flash memory so that values survive **power cycles and resets**.

---

### 📌 Flash Address Used

```bash
#define FLASH_BASE_ADDR       0x08060000U
#define FLASH_SECTOR_TO_ERASE FLASH_SECTOR_7
```
* Address: 0x08060000 corresponds to Sector 7 of Flash memory.

* Sector Size: 128 KB

* MCU: STM32F446RE

## ⚠️ Flash Memory Layout

| Sector | Address        | Size                                  |
| ------ | -------------- | ------------------------------------- |
| 0      | 0x08000000     | 16 KB                                 |
| 1–3    | ...            | 16 KB                                 |
| 4      | 0x08010000     | 64 KB                                 |
| 5–6    | 0x08020000+    | 128 KB                                |
| **7**  | **0x08060000** | **128 KB ← Used for persistent data** |

## ⚠️ Firmware Flash Size Limit
Make sure your application does not exceed 0x08060000 in Flash.

* ✅ Option 1: Modify the .ld linker script to cap the program size.
* ✅ Option 2: In STM32CubeIDE, go to

```bash
Project > Properties > C/C++ Build > Settings > MCU Settings
```
and set Flash size limit appropriately.

## ✅ Behavior
On startup, the firmware reads register values from Flash (0x08060000) into RAM.

* On executing:
```bash
yakut execute-command 96 store
```
⇒ Register values are written to Flash.

* On executing:

```bash
y cmd 96 restart
```
⇒ MCU performs a software reset.

* Flashing new firmware does not erase stored values
unless Sector 7 is manually erased.

## 🧯 Factory Reset (Optional)
To reset the register values to factory defaults:

* 🛠 Manual: Erase Sector 7 using STM32CubeProgrammer.

* 🔁 Software: Implement a factory_reset command handler that:

  * Erases Sector 7

  * Resets the MCU (via NVIC_SystemReset())

## Help

Contact the [Authors](##authors), but please make sure to make an effort before asking for help.

## Authors

Contributor names and contact info

Md Tuhin Ahmed | [ahmedmd.tuhin@yahoo.com](ahmedmd.tuhin@yahoo.com)



## Version History

* Version 0.01
* I really hope it works.

## License

I have not yet started on licences.

## Acknowledgments

Thanks to [awesome-readme](https://github.com/matiassingers/awesome-readme) for the ReadMe-Template.
