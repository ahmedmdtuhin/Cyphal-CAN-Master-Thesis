# Master's Thesis - Embedded Cyphal Projects

This repository contains multiple embedded software projects developed as part of my Master's thesis. Each project demonstrates key elements of real-time CAN communication using the Cyphal protocol on STM32 microcontrollers.

---

## Repository Purpose

The aim of this repository is to explore and implement various features of the **Cyphal/CAN protocol** within embedded systems. The focus is on:

- Efficient message transmission via CAN using Cyphal
- Interrupt-based message handling
- Remote register interaction
- Service-command handling
- Multi-frame and array message publishing/subscribing

Each sub-project refines or builds upon a previous implementation to incrementally achieve the final design goals.

---

## Final Project Version

The final version of this repository is located in:

```
Cyphal_CAN_implementation_with_IRQ_list_Access2_ExecuteCommand/
```

This final project includes the following features:

- Full **Cyphal Node** implementation
- **Interrupt-driven** CAN message handling
- Register access and modification using `yakut`
- Support for **ExecuteCommand** service (e.g., node restart, store persistent state)
- Array publishing and subscribing using subject-ID `1620`
- Integration with standard DSDL definitions from the OpenCyphal public regulated types

---

## Subdirectories

Each subdirectory represents a stage or feature focus in the development process. Some directories are early prototypes, while others are partial feature implementations.

---

## How to Use

Refer to the `README.md` inside the final project folder for detailed setup, configuration, and usage instructions.

If you plan to interact with the node from a Linux PC, ensure the following tools and environment are set up:

- `yakut`
- `socketcan` with proper CAN interface setup
- DSDL namespace configuration

---

## Author

**Md Tuhin Ahmed**  
Email: [ahmedmd.tuhin@yahoo.com](mailto:ahmedmd.tuhin@yahoo.com)

---

## License

This repository is intended for academic and research purposes. Contact the author if you intend to reuse or adapt parts of the project for commercial or derivative work.
