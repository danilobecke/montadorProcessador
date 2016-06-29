# montadorProcessador

Montador que gera um arquivo .mif para pre-carregar uma memória. Ele aceita as instruções:

|   Operação  |    Função executada   | Opcode |
|:-----------:|:---------------------:|:------:|
|  mv Rx, Ry  |       Rx <- [Ry]      |   000  |
|  mvi Rx, #D |        Rx <- D        |   001  |
|  add Rx, Ry |   Rx <- [Rx] + [Ry]   |   010  |
|  sub Rx, Ry |   Rx <- [Rx] - [Ry]   |   011  |
| ld Rx, [Ry] |      Rx <- [[Ry]]     |   100  |
| st Rx, [Ry] |      [Ry] <- [Rx]     |   101  |
| mvnz Rx, Ry | if G != 0, Rx <- [Ry] |   110  |
