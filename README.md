# D1 Mini OLED 64x48 NTP Clock (São Paulo, Brasil)

Projeto Arduino/ESP8266 desenvolvido para placa **WEMOS D1 Mini (ESP8266EX)** com **Wemos OLED Shield 64x48 (I2C SSD1306)**.

O dispositivo se conecta à rede Wi-Fi, sincroniza data e hora oficial de Brasília/São Paulo (`UTC-3`) via servidores NTP brasileiros (`a.st1.ntp.br`, `b.st1.ntp.br`) e exibe continuamente as informações no visor OLED usando a biblioteca gráfica **U8g2**.

## 📌 Características
- **Microcontrolador**: ESP8266EX (WEMOS D1 Mini)
- **Display**: OLED monocromático 64x48 pixels (SSD1306)
- **Barramento**: I2C nos pinos nativos D2 (SDA / GPIO4) e D1 (SCL / GPIO5)
- **Driver Gráfico**: Biblioteca `U8g2` (`U8G2_SSD1306_64X48_ER_F_HW_I2C`)
- **Sincronização de Tempo**: NTP com fuso horário POSIX `<-03>3` (sem horário de verão)
- **Layout de Tela**:
  - Topo: Data centralizada no formato `DD/MM/AA`
  - Centro: Horário `HH:MM` em destaque com segundos `:SS`
  - Rodapé: Identificação regional `SP - Brasil` e status de conectividade

## 📄 Documentação
Consulte o [FSD.md](FSD.md) para a Especificação Funcional Completa (Functional Specification Document).
