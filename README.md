# ZypheraMesh

A radio-agnostic decentralized mesh networking stack for embedded systems, enabling robust peer-to-peer communication across diverse hardware platforms.

## Overview

This project provides a lightweight, hardware-independent mesh networking layer designed for resource-constrained embedded devices. By abstracting the physical radio layer, it allows the same networking code to run on different radio transceivers (LoRa, nRF24, ESP-NOW, etc.) without modification.

### Key Features

- **Radio Agnostic**: Hardware abstraction layer supports multiple radio modules
- **Decentralized**: No central coordinator or access point required
- **Self-Healing**: Automatic route discovery and maintenance
- **Lightweight**: Optimized for microcontrollers with limited RAM/flash
- **Scalable**: Designed to handle networks from 2 to 100+ nodes
- **Low Power**: Sleep mode support and energy-efficient routing

## Use Cases

- IoT sensor networks
- Emergency communication systems
- Off-grid mesh networks
- Agricultural monitoring
- Smart building automation
- Disaster recovery communications
- Custom data packets

## Supported Hardware

### Radio Modules
- LoRa (SX1276, SX1278, SX1262)
- nRF24L01+
- HC-12
- ESP-NOW (ESP32/ESP8266)
- CC1101
- Custom radio implementations via HAL

### Microcontrollers
- ESP32/ESP8266
- STM32 (ARM Cortex-M)
- Arduino (AVR, SAMD)
- Raspberry Pi Pico (RP2040)
- Nordic nRF52 series

## Installation

### Prerequisites

- **Compiler**: GCC ARM cross-compiler or platform-specific toolchain
- **Build System**: CMake 3.15+ or PlatformIO
- **Dependencies**: Platform HAL/SDK for your target hardware

### Building

#### Using CMake

```bash
mkdir build && cd build
cmake .. -DTARGET_PLATFORM=esp32 -DRADIO_TYPE=lora
make
```

#### Using PlatformIO

```bash
pio run -e esp32-lora
```

#### Configuration Options

| Option | Description | Default |
|--------|-------------|---------|
| `TARGET_PLATFORM` | Target microcontroller platform | `esp32` |
| `RADIO_TYPE` | Radio module type | `lora` |
| `MAX_NODES` | Maximum nodes in network | `64` |
| `ENABLE_ENCRYPTION` | Enable AES encryption | `ON` |
| `ENABLE_DEBUG` | Debug logging | `OFF` |

## Quick Start

### 1. Initialize the Stack

```cpp
#include "mesh_network.h"

// Create network instance with node ID
MeshNetwork mesh(0x0001);

// Configure radio parameters
RadioConfig config;
config.frequency = 915000000;  // 915 MHz
config.bandwidth = 125000;     // 125 kHz
config.spreading_factor = 7;
config.tx_power = 14;          // dBm

mesh.init(config);
```

### 2. Send Messages

```cpp
// Broadcast to all nodes
mesh.broadcast("Hello mesh!");

// Send to specific node
mesh.sendTo(0x0002, "Direct message", 14);

// Send with routing (multi-hop)
mesh.routeTo(0x0005, data, length);
```

### 3. Receive Messages

```cpp
void onMessageReceived(uint16_t from, const uint8_t* data, size_t len) {
    Serial.printf("From node 0x%04X: %.*s\n", from, len, data);
}

mesh.onReceive(onMessageReceived);
```

### 4. Main Loop

```cpp
void loop() {
    mesh.update();  // Process network events
    delay(10);
}
```

## Architecture

### Network Layers

```
┌─────────────────────────────┐
│   Application Layer         │
├─────────────────────────────┤
│   Routing Layer (AODV)      │
├─────────────────────────────┤
│   Network Layer             │
├─────────────────────────────┤
│   Radio Abstraction (HAL)   │
├─────────────────────────────┤
│   Physical Radio Hardware   │
└─────────────────────────────┘
```

### Routing Protocol

Uses a modified AODV (Ad-hoc On-Demand Distance Vector) protocol optimized for:
- Low memory footprint
- Minimal broadcast overhead
- Fast route discovery
- Automatic route repair

## API Reference

### Core Classes

#### `MeshNetwork`
Main network interface class.

```cpp
class MeshNetwork {
public:
    MeshNetwork(uint16_t nodeId);
    bool init(const RadioConfig& config);
    void update();
    
    // Messaging
    bool broadcast(const void* data, size_t len);
    bool sendTo(uint16_t dest, const void* data, size_t len);
    bool routeTo(uint16_t dest, const void* data, size_t len);
    
    // Callbacks
    void onReceive(MessageCallback callback);
    void onRouteDiscovered(RouteCallback callback);
    
    // Network info
    uint8_t getHopCount(uint16_t dest);
    uint8_t getNeighborCount();
    const Node* getNeighbors();
};
```

### Configuration

#### `RadioConfig`
Hardware-agnostic radio parameters.

```cpp
struct RadioConfig {
    uint32_t frequency;
    uint32_t bandwidth;
    uint8_t spreading_factor;
    int8_t tx_power;
    uint16_t preamble_length;
    bool crc_enabled;
};
```

## Advanced Usage

### Custom Radio Driver

Implement the `RadioHAL` interface for custom radio hardware:

```cpp
class CustomRadio : public RadioHAL {
public:
    bool init(const RadioConfig& config) override;
    bool send(const uint8_t* data, size_t len) override;
    bool receive(uint8_t* buffer, size_t& len) override;
    bool setChannel(uint8_t channel) override;
    void sleep() override;
};
```

### Encryption

Enable AES-128 encryption for secure communications:

```cpp
mesh.enableEncryption(true);
mesh.setEncryptionKey(key, 16);
```

### Power Management

```cpp
// Put radio to sleep between transmissions
mesh.setSleepMode(true);
mesh.setWakeInterval(5000);  // Wake every 5 seconds
```

## Performance

| Metric | Value |
|--------|-------|
| Latency (single hop) | 50-200ms |
| Latency (multi-hop) | +100ms per hop |
| Throughput | 1-50 kbps (radio dependent) |
| Max network size | 254 nodes |
| RAM usage | 2-8 KB |
| Flash usage | 20-40 KB |

## Examples

See the `examples/` directory for complete projects:

- `basic_node/` - Simple two-node communication
- `sensor_network/` - Multi-sensor data collection
- `gateway/` - Internet gateway bridge
- `mobile_node/` - Battery-powered mobile mesh node

## Testing

### Unit Tests

```bash
cd test
mkdir build && cd build
cmake ..
make
./run_tests
```

### Network Simulation

```bash
python tools/network_simulator.py --nodes 10 --topology random
```

## Troubleshooting

### Nodes not discovering each other
- Verify radio frequencies match
- Check antenna connections
- Ensure nodes are within range
- Enable debug logging to see discovery packets

### High packet loss
- Reduce TX power if nodes too close
- Increase spreading factor for longer range
- Check for RF interference
- Verify CRC is enabled

### Routes not forming
- Check MAX_NODES setting
- Verify sufficient RAM available
- Enable routing debug output
- Check node IDs are unique

## Contributing

Contributions welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Code Style
- Follow Google C++ Style Guide
- Use 4 spaces for indentation
- Maximum line length: 100 characters
- Document all public APIs

## Roadmap

- [ ] IPv6 over mesh support
- [ ] Time synchronization protocol
- [ ] Mesh diagnostics and monitoring
- [ ] Android/iOS bridge apps
- [ ] Web-based network visualizer
- [ ] Support for additional radio modules

## License

MIT License - see [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by B.A.T.M.A.N. and Meshtastic projects
- AODV protocol implementation based on RFC 3561
- Thanks to the embedded mesh networking community

## Support

- **Documentation**: [docs/](docs/)
- **Issues**: [GitHub Issues](https://github.com/yourusername/mesh-network/issues)
- **Discussions**: [GitHub Discussions](https://github.com/yourusername/mesh-network/discussions)
- **Discord**: [Community Server](https://discord.gg/yourserver)

## Citation

If you use this project in research, please cite:

```bibtex
@software{mesh_network_stack,
  title = {Radio-Agnostic Mesh Network Stack},
  author = {Your Name},
  year = {2024},
  url = {https://github.com/yourusername/mesh-network}
}
```
