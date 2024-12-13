# Renan Sabino dos Reis

# Descrição 

## **1. Estados do Sistema**

### 1. **Modo Noturno (LDR detecta ambiente escuro)**
- **Descrição**: Quando a luminosidade medida pelo LDR é menor ou igual ao valor do threshold, o sistema entra no modo noturno.  
  - O LED amarelo (led_yellow) pisca em intervalos de 500 ms.  
  - O comportamento dos LEDs verde e vermelho é interrompido temporariamente.

### 2. **Modo Claro (LDR detecta ambiente claro)**  
- **Descrição**: Quando a luminosidade medida pelo LDR é maior que o threshold, o sistema segue a temporização dos LEDs, simulando o controle do semáforo.  
    - O sistema entra em uma rotina de temporização dos LEDs, que representa o funcionamento padrão de um semáforo.  
  - Possui gerenciamento de debounce para o botão (buttonPin) para evitar leituras falsas.

#### 2.1 **Ações no Modo Claro**
 
**Botão Pressionado Durante o LED Vermelho** 
   - **Descrição**:  
     - **Um Pressionamento**:Após 1 segundo do botão pressionado, o semáforo muda para o LED verde por 3 segundos.  
     - **Três Pressionamentos**: Envia um alerta HTTP.  

### 3. **Estado de Envio de Alerta HTTP**
- Ocorre após 3 pressionamentos consecutivos do botão.  
- **Descrição**: Envia uma requisição HTTP para um endereço especificado (http://www.google.com.br/).  

  - **Sucesso**: Requisição enviada, código de resposta: [httpCode]

  - **Falha**: Erro na requisição: [mensagem de erro]

  - **WiFi Desconectado**: WiFi não está conectado.