# Trabalho-3-CSD

TESTE:
  O primeiro exemplo (VGA Demo) ilustra o uso de todas as funcionalidades do driver de v´ıdeo e GDI (Graphics Display Interface), que inclui
uma API para acesso `as fun¸c˜oes gr´aficas. O segundo exemplo ilustra o uso
de processamento em 3D utilizando c´alculos por software e o terceiro exemplo mostra um exemplo de jogo, contendo uma bola, objetos, detec¸c˜ao de
colis˜ao e controles dos bot˜oes. O ´ultimo exemplo apresenta uma forma de
construir um jogo utilizando sprites. Os exemplos podem ser compilados
com o comando make nome exemplo F CLK=50000000, e carregados para
a placa FPGA com o comando make load SERIAL DEV=/dev/ttyUSB1.
Antes de carregar a aplica¸c˜ao, recomenda-se que em um terminal seja utilizado para depura¸c˜ao, sendo que sua aplica¸c˜ao poder´a utilizar comandos
de impress˜ao. Utilize o comando make debug SERIAL DEV=/dev/ttyUSB1
para isso. Assume-se que o script que configura as permiss˜oes de acesso `a
porta serial tenha sido executado. Se nada aparecer no terminal, programe
a placa e pressione o bot˜ao CPU RESET (dever´a aparecer uma mensagem
do bootloader ).
O adaptador gr´afico possui uma s´erie de limita¸c˜oes, sendo que o hardware
gera um sinal VGA padr˜ao, com resolu¸c˜ao de 640x480 pixels compat´ıvel com
qualquer monitor LCD ou CRT. Em fun¸c˜ao das limita¸c˜oes na plataforma
de prototipa¸c˜ao, e escolha deliberada de uma pequena mem´oria de v´ıdeo de
32KB, a resolu¸c˜ao do adaptador ´e de apenas 300x218 pixels com 16 cores
(4 bits por pixel). Toda a renderiza¸c˜ao de objetos ´e realizada por software,
portanto ser´a necess´ario cuidado ao atualizar os objetos para minimizar
o n´umero de pixels a serem modificados, com o objetivo de se obter um
desempenho aceit´avel no jogo. A mem´oria de v´ıdeo ´e uma mem´oria de
porta simples, sendo seu acesso multiplexado pelo controlador gr´afico e a
CPU2
.


TECLAS:
A - left
D - right
K - shoot
