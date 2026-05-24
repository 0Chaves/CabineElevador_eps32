# CabineElevador_esp32

## Visao geral

Este projeto representa a cabine/controlador do elevador. Ele trabalha em conjunto com o projeto `HallElevador_esp32`, que representa o painel externo do hall.

A comunicacao entre os dois ESP32 acontece por MQTT:

- `HallElevador_esp32` envia chamadas de andar quando um botao externo e pressionado.
- `CabineElevador_esp32` recebe essas chamadas, registra o andar de destino e controla o movimento do elevador.
- `CabineElevador_esp32` publica o estado atual da cabine para que o hall consiga atualizar seus LEDs.
- `HallElevador_esp32` recebe o estado publicado pela cabine e indica ao usuario o andar atual e a chegada do elevador.

Ambos os projetos usam o mesmo broker MQTT:

| Configuracao | Valor |
| --- | --- |
| Broker | `192.168.1.114` |
| Porta | `1883` |
| Biblioteca MQTT | `PubSubClient` |
| Formato das mensagens | JSON |

## Fluxo de interacao

1. O usuario pressiona um botao no hall.
2. O `HallElevador_esp32` publica uma mensagem no topico `grupo5/hall/chamada_andar`.
3. A `CabineElevador_esp32`, inscrita nesse topico, recebe o andar solicitado.
4. A cabine valida o andar recebido, marca esse andar como destino e executa o movimento do elevador.
5. Ao final do ciclo de movimento, a cabine publica seu estado no topico `grupo5/elevador/estado`.
6. O hall, inscrito nesse topico, atualiza os LEDs conforme o andar atual, o status da porta e a chegada do elevador.

## Topicos MQTT

| Projeto | Publica | Inscreve |
| --- | --- | --- |
| `HallElevador_esp32` | `grupo5/hall/chamada_andar` | `grupo5/elevador/estado` |
| `CabineElevador_esp32` | `grupo5/elevador/estado` | `grupo5/hall/chamada_andar` |

## Inscricao da Cabine nas Chamadas do Hall

### Topico

`grupo5/hall/chamada_andar`

### Quando recebe

A cabine recebe mensagens nesse topico sempre que um botao do painel do hall e pressionado.

No codigo da cabine, os andares validos vao de `0` a `3`:

| Valor | Andar |
| --- | --- |
| `0` | Terreo |
| `1` | Segundo andar |
| `2` | Terceiro andar |
| `3` | Quarto andar |

### Payload recebido

```json
{
  "andarDestino": 2
}
```

### Campos

| Campo | Tipo | Descricao |
| --- | --- | --- |
| `andarDestino` | inteiro | Andar chamado pelo usuario no painel do hall. Deve estar entre `0` e `3`. |

### O que a cabine faz ao receber

A `CabineElevador_esp32` esta inscrita em `grupo5/hall/chamada_andar`. Quando recebe a mensagem:

1. Le o campo `andarDestino`.
2. Verifica se o valor esta entre `0` e `ULTIMO_ANDAR`.
3. Marca esse andar como destino usando `elevador.setAndarDestino(andarDestino)`.
4. O metodo `moverElevador()` usa essa chamada para decidir se o elevador deve subir, descer ou permanecer parado.

## Publicacao da Cabine para o Hall

### Topico

`grupo5/elevador/estado`

### Quando publica

A cabine publica nesse topico ao final de cada ciclo do `loop()`, depois de:

1. verificar a conexao Wi-Fi e MQTT;
2. processar mensagens recebidas pelo `mqttClient.loop()`;
3. ler os botoes internos da cabine;
4. executar o movimento do elevador com `elevador.moverElevador()`;
5. montar o JSON com o estado atual da cabine.

### Payload enviado

```json
{
  "andar_atual": 2,
  "porta_status": 1,
  "elevador_chegada": true
}
```

### Campos

| Campo | Tipo | Descricao |
| --- | --- | --- |
| `andar_atual` | inteiro | Andar onde a cabine esta parada ou acabou de chegar. Usa valores de `0` a `3`. |
| `porta_status` | inteiro | Estado da porta lido pelo sensor magnetico. `0` significa fechada e `1` significa aberta. |
| `elevador_chegada` | booleano | Indica se o elevador chegou a um destino neste ciclo. `true` significa chegada confirmada; `false` significa que nao houve chegada. |

### O que o hall faz ao receber

O `HallElevador_esp32` esta inscrito em `grupo5/elevador/estado`. Quando recebe a mensagem:

1. Le `andar_atual`, `porta_status` e `elevador_chegada`.
2. Atualiza os atributos internos do objeto `Hall`.
3. Liga o LED correspondente ao andar atual da cabine.
4. Se `elevador_chegada` for `true` e `porta_status` for `1`, aciona o LED extra de chegada por alguns segundos.

## Botoes internos da cabine

A cabine tambem possui botoes proprios. Quando um botao interno e pressionado, o projeto nao precisa publicar uma chamada MQTT para si mesmo. Ele registra diretamente o destino com:

```cpp
elevador.setAndarDestino(i);
```

Assim, a cabine aceita chamadas de duas origens:

- chamadas externas recebidas pelo MQTT no topico `grupo5/hall/chamada_andar`;
- chamadas internas lidas diretamente pelos botoes conectados ao ESP32 da cabine.

## Responsabilidade de cada projeto

### `CabineElevador_esp32`

- Recebe chamadas vindas do hall.
- Le botoes internos da cabine.
- Controla o movimento do elevador.
- Le o sensor magnetico da porta.
- Atualiza os LEDs internos conforme o andar atual.
- Publica o estado atual da cabine no MQTT.

### `HallElevador_esp32`

- Le os botoes externos do hall.
- Publica chamadas de andar no MQTT.
- Recebe o estado da cabine.
- Atualiza os LEDs do hall com base no estado recebido.

## Resumo

Assim, os dois projetos se complementam: o hall solicita destinos e a cabine executa o movimento, atualiza seus sensores/atuadores e devolve seu estado para que o hall consiga mostrar ao usuario onde o elevador esta e quando ele chegou.
