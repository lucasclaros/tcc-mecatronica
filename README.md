# Projeto Estufa Automatizada com IOT
Este foi meu Trabalho de Conclusão de Curso Técnico em Mecatrônica feito em 2019. Consiste em um sistema de irrigação automático para uma estufa caseira, podendo ser controlada
por um aplicativo .apk ou uma página web construida para isso.

<br />

## * O Projeto

Há duas versões desse projeto: uma feita para apresentação na escola e a para uso em casa. A diferença entre as duas está na possibilidade de linkar com a plataforma Blynk na versão para casa,
além de claro o SSID da rede wifi em que ele vai se conectar. Para o aplicativo, também há duas versões: uma configurada para um servidor local (apresentado na escola) e outra que está configurada para fazer a conexão dns direto da minha casa. Ambos foram feitos a partir da plataforma [MIT App Inventor].

## * Funcionamento

O programa configuras os servidores que estão armazenados cada tipo de dado: sensor de temperadura, umidade estado atual da bomba de água que irá irrigar e o estado da lampada de iluminação que ajuda no crescimento da plantação.
<br />
O aplicativo está disponível [aqui], sinta-se a vontade para modificá-lo/melhorá-lo e também adaptá-lo ao seu projeto colocando seu próprio servidor local como endereço url para consulta de dados.

Seguindo as orientações contidas nos arquivos "nodemcu.png" e "pinagem nodemcu.txt", os sensores foram conectados no seguinte esquema:

* [Sensor de Umidade]: pino A0 



[MIT App Inventor]: http://ai2.appinventor.mit.edu/
[aqui]: https://gallery.appinventor.mit.edu/?galleryid=43e643e5-a1fc-4e42-b63c-fa65002ae7c7
[Sensor de Umidade do Solo]: https://www.filipeflop.com/produto/sensor-de-umidade-do-solo-higrometro/
[Sensor de Temperatura DHT22]: https://www.filipeflop.com/produto/sensor-de-umidade-e-temperatura-am2302-dht22/
[Bomba de Água]: https://produto.mercadolivre.com.br/MLB-1420976166-mini-bomba-da-agua-12v-pulverizaco-robotica-_JM#position=29&search_layout=stack&type=item&tracking_id=55ae6545-3667-49c3-bec7-0f0373c7ab7e
[LED para Cultivo]: https://produto.mercadolivre.com.br/MLB-1577644835-led-grow-cultivo-indoor-estufa-fullspectrum-28w-uvir-planta-_JM