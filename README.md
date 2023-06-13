# 1 INTRODUÇÃO

## 1.1 Justificativa

Este trabalho se justifica por dar mais controle ao morador sobre a abertura e fechamento de seu portão de casa. Ao final do desenvolvimento, será possível controlar de forma remota via aplicativo de celular, o envio de comandos a um emissor de sinal, além do recebimento de notificações sobre a mudança de estado (aberto ou fechado).

## 1.2 Definição do problema

O conceito de internet das coisas é relativamente novo, criado em 1999 por Kevin Ashton ao criar um sistema com RFID para rastrear um produto em uma cadeia de suprimentos (revista FACTEQ 2023).

Desde então foram desenvolvidos outros produtos para facilitar o dia a dia das pessoas, criando um controle cada vez maior das pessoas sobre os objetos que estão a sua volta. 

Eventualmente saímos de casa e nos deparamos com situações de que podemos ter esquecido o portão aberto, ou de poder liberar o acesso a alguém sem termos que procurar a chave ou controle, ou até mesmo sem estarmos em casa. Assim, surge a seguinte pergunta: “Como criar um sistema que possa controlar à distância os estados (aberto e fechado) de um portão eletrônico comum?”.

## 1.3 Objetivo geral

Desenvolver um sistema de comunicação entre o portão eletrônico e um smartphone, onde uma câmera capta imagens do portão e repassa a um microcontrolador, este por sua vez analisará se o portão não está fechado por completo por um período determinado de tempo, e se comunicará com o smartphone via sinal de wifi onde o usuário irá escolher qual ação deve ser tomada. Este fluxo é ilustrado pelo diagrama de blocos a seguir:
	
<p align="center">
  <img src="https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/bf5ebd5d-215c-4fb8-baf3-ad1f694dfba9">
</p>

Koyanagi (2019), desenvolveu um projeto utilizando ESP32-CAM para reconhecimento facial, mostrando ser possível utilizar as imagens capturadas através da câmera OV2640 / 2 mp para identificar pessoas através de seus rostos, e este mesmo princípio pode ser utilizado para identificar as situações do portão eletrônico.

## 1.4 Objetivos específicos

Este trabalho tem os seguintes objetivos específicos:

a) Definir os componentes;

b) Comprar os componentes;

c) Montar o circuito;

d) Desenvolver o firmware;

e) Desenvolver o aplicativo;

f) Fabricar a placa de circuito.


# 2 REVISÃO DE LITERATURA

Neste capítulo será feita abordagem dos conceitos relacionados ao tema para fundamentar a análise com teorias elaboradas por diversos autores.

## 2.1 Conceito de internet das coisas

Segundo Kiane (2019), com o aumento da disponibilidade de internet de banda larga e a incorporação de sensores em dispositivos com conexão Wi-Fi, houve uma redução no custo dessas tecnologias, levando à popularização de smartphones e criando um cenário favorável para a Internet das Coisas (IoT). A IoT consiste em conectar virtualmente qualquer dispositivo à internet, incluindo componentes de máquinas, formando uma rede de "coisas" conectadas, incluindo pessoas, permitindo relacionamentos entre pessoas, coisas e pessoas-coisas.

## 2.2 Semicondutores, condutores e isolantes

Semicondutores são materiais onde sua resistência está entre a dos isolantes e dos condutores, sendo os de uso mais comum o germânio e o silício (CIPELLI; MARKUS; SANDRINI, 2005).

Cipelli, Markus e Sandrini (2005, p. 4) afirmam que 
 “Os condutores são elementos que possuem elétrons livres em grande quantidade e fracamente ligados ao núcleo, sendo que, sob a ação de uma diferença de potencial, passam a ser locomover no interior do material condutor. Quanto maior o número de elétrons livres, maior será o fluxo de corrente e, consequentemente, maior será sua condutividade.”
 
Nos isolantes, os elétrons encontram-se fortemente ligados ao núcleo, e mesmo aquecendo-os, pouca quantidade destes tornam-se livres, o que dificulta o fluxo de corrente em uma diferença de potencial (CIPELLI; MARKUS; SANDRINI, 2005).

## 2.3 Fenômenos transientes

Fenômenos transientes “São aqueles gerados por fontes internas e externas ao sistema supridor, com diferentes formas de onda e de variada intensidade, que afetam o funcionamento dos equipamentos de tecnologia da informação [...]”. Esses fenômenos podem causar danos permanentes (MAMEDE FILHO, 2010, p. 168).

## 2.4 Blindagem eletromagnética

Blindagem eletromagnética significa reduzir os campos eletromagnéticos que atuam sobre uma instalação e/ou sobre seus componentes. A blindagem é entendida como uma barreira de metal entre uma fonte emissora de ondas eletromagnéticas, e um ambiente, equipamento ou algo que queira imunizar contra interferências que possam afetar negativamente o desempenho dos aparelhos (MAMEDE FILHO, 2010).

# 3 METODOLOGIA

Para avaliação do tema deste trabalho, serão utilizados procedimentos metodológicos que auxiliarão na forma de pesquisa e documentação para atingir os objetivos propostos do estudo em questão.

Quanto à natureza, esta pesquisa terá caráter de pesquisa aplicada, por ter como objetivo o desenvolvimento da pesquisa pela prática, visando solucionar problemas específicos, e envolvendo interesses e verdades locais.

A pesquisa deste trabalho por ter embasamento na realidade de uma organização, na busca pela compreensão do fenômeno particular e no envolvimento frequente de pesquisas de campo, será considerada qualitativa (RAUEN, 2002).

Em relação aos objetivos, a pesquisa será exploratória por tornar o problema explícito, por envolver o levantamento de dados bibliográficos, por buscar conhecimento com pessoas que já vivenciaram experiências práticas com o problema em questão, e por analisar problemas e soluções semelhantes que auxiliarão na compreensão deste problema.

Por ser elaborada com a utilização de material publicado, seja em livros, internet e/ou artigos, esta pesquisa será considerada como bibliográfica.

# 4 CRONOGRAMA

<p align="center">
  <img src="https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/2bce7f96-51e6-4d30-b9a1-274b8e7e873c">
</p>

# 5 DESENVOLVIMENTO

## 5.1 Linguagem e IDE de desenvolvimento

Para realizar o projeto, após pesquisa, resolvi optar por comprar o microcontrolador ESP32-CAM que é um modelo amplamente utilizado no mundo inteiro, que possui uma boa documentação, atende aos requisitos de projeto (criar um webserver, tirar fotos e analisar as imagens).
Decidi comprar duas placas iguais (para caso uma delas queimasse durante os testes), e uma placa um pouco mais robusta (ESP32-CAM Wrover).

Com as placas em mãos, iniciei com o plano de utilizar o Micropython, segui alguns tutoriais diferentes e em todos eles eu consegui fazer o upload do firmware, mas ao abrir o programa Thonny, sempre me deparava om a mensagem de que a porta COM estava ocupada. Tentei buscar a solução deste problema por aproximadamente três dias, desisti e segui por um outro caminho, programar em C/C++ utilizando o Visual Studio Code.

Com o Visual Studio Code, consegui rodar alguns exemplos, testei o uso da extensão PlataformIO e também da ESP-IDF, porém diversas vezes me deparava com o problema de o software travar o carregamento e consequentemente me atrasando no desenvolvimento.

Após conversa com um colega de sala, ele sugeriu o uso da IDE ESP-IDF para desenvolver o firmware. Utilizei de alguns exemplos do Github oficial da Espressif como base para o desenvolvimento do projeto, tive aqui também alguns problemas de travamento, porém consegui resolvê-los com maior rapidez.

## 5.2 Firmware e aplicativo

Iniciei o desenvolvimento do firmware configurando o wifi no microcontrolador, depois configurei a câmera e webserver para visualizar as fotos e transmissão do vídeo. Quando finalizei esta etapa, iniciei o desenvolvimento da comunicação entre o microcontrolador e o smartphone.

Utilizei o Visual Studio Code para desenvolver o aplicativo com o framework Flutter, que utiliza a linguagem DART. Escolhi o Flutter pois me permite desenvolver um único código para ser utilizado nas plataformas Android e IOS. Criei o aplicativo com apenas um botão e que exibe em sua tela uma webview com as imagens geradas pelo microcontrolador. Abaixo está a foto do aplicativo:

<p align="center">
  <img src="https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/a4402aa8-267d-4bf2-bc72-8e66ba76feb0">
</p>

Para receber os comandos de abrir/fechar o portão, configurei no aplicativo o serviço de envio de mensagens do Firebase, conforme ilustrado abaixo:

<p align="center">
  <img src="https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/98cb3e4d-4fa9-4493-99a0-1ddcaa0d7192">
</p>

Para validar o funcionamento deste serviço, utilizando o softaware Postman simulando o envio de requisição HTTPS ao Firebase, e este por sua vez direciona a mensagem ao aplicativo. A próxima imagem mostra um exemplo desta comunicação.

<p align="center">
  <img src="https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/8870520b-e316-47cd-b078-2b8f8d4a584d">
</p>

Após validar o funcionamento da comunicação, foi feito o desenvolvimento desta requisição no microcontrolador. Na sequência, desenvolvi no aplicativo o envio de uma requisição (não segura) para o microcontrolador no momento que o usuário clicar no botão "abrir/fechar", e no microcontrolador o recebimento desta mensagem. As figuras a seguir ilustram o código do smartphone e a confirmação do recebimento dela no microcontrolador, consequentemente:

<p align="center">
  <img src="https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/b449ed27-722a-4d47-97af-85bca2e961ca">
</p>

<p align="center">
  <img src="https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/8b656695-8665-4ce1-96fc-8d357f658a01">
</p>

## 5.3 Processamento das imagens

Com a comunicação praticamente completa, restou a parte mais importante deste projeto por último, o reconhecimento dos estados (aberto e fechado) do portão. Para isso, foi inicialmente desenvolvida uma API em python para validação da análise, utilizando a biblioteca Opencv, que comparava a atual imagem com outras duas, uma do portão aberto e outra do portão fechado. A biblioteca se mostrou eficaz, porém para uma redução nos custos, o ideal seria que essa comparação fosse realizada dentro do microcontrolador, e não em um servidor.

Procurei pela mesmo biblioteca Opencv porém destinada ao esp32-cam. Encontrei, adicionei ao código do firmware, estudei alguns exemplos e iniciei o desenvolvimento. Consegui utilizar a função que trocar as cores da imagem para a escala de cinza, porém não consegui converter a imagem a um formato que me permitisse visualizar a imagem. Boa parte das funções da biblioteca oficial não estavam presentes na versão do microcontrolador e também não existe uma documentação específica para o seu uso. Então decidi tentar uma nova abordagem, instalar o micropython.

Realizei diversas pesquisas para solucionar o problema que eu estava tendo ao tentar utilizar o Micropython de que a porta COM estava ocupada, porém nada funcionava. Então resolvi testar um procedimento de baixar o firmware utilizando o software Thonny na placa esp32-cam Wrover, e o procedumento funcionou. Realizei alguns testes de conectar na internet e configurar o webserver, e depois disso, etava tentando colocar dentro do firmware do Micropython tudo que eu havia feito em C/C++, e no meio desta pesquisa encontrei um relato de uma pessoa que estava com o mesmo problema que eu da porta COM ocupada, e a solução do problema era acessar as configurações do Thonny e adicionar duas linhas de código, e isso funcionou.

Coloquei o Micropython na placa esp32-cam e fiz alguns testes e depois pesquisei sobre a compatibilidade com a biblioteca Opencv, porém desta vez não encontrei nem a biblioteca mas sim comentários de que não era possível utilizá-la, então retornei a estaca zero. Minhas opções agora seriam:

a) Voltar a tentar utilizar a biblioteca Opencv para adaptada para o esp32;

b) Procurar por uma nova biblioteca que faça um processamento de imagens dentro do esp32;

Segui pela segunda opção, encontrei a mesma biblioteca Opencv, porém em JavaScript para uso dentro de páginas web. O site https://randomnerdtutorials.com/esp32-cam-opencv-js-color-detection-tracking/ mostra um exemplo da utilização no arduino, e estou utilizando e adaptando a webpage para processar a imagem conforme minha necessidade.

<p align="center">
  <img src="https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/80c2ea23-36fb-43eb-b822-697f320a1303">
</p>

Seguindo o exemplo acima, realizei um teste para identificar um objeto na cor vermelha, pois as cores vermelho, verde e azul são mais fáceis de se identificar dentro de um ambiente. Colei um material EVA da cor vermelha no portão e tentei identificá-lo nas imagens, porém sem sucesso, muito provavelmente pela baixa qualidade das fotos, distância entre o material e a câmera, e uma iluminação inadequada. Abaixo está a foto do material inserido no portão.

<p align="center">
  <img src="https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/f26d7146-f009-45f0-90c4-4510a03e911e">
</p>

Fiz um novo código para captar as imagens do portão através do microcontrolador e salvá-las no cartão de memória. Isto me permitirá consultar as imagens salvas após processá-las para validar se o resultado está sendo o esperado.

As imagens foram anexadas neste projeto, elas foram sendo capturadas em um intervalo de aproximadamente 10 minutos. Ao analisá-las, foi verificado que enquanto anoitecia, a visualização foi ficando cada vez mais prejudicada, o que deve tornar a verificação dos estados do portão ainda mais difícil. Abaixo estão algumas das fotos tiradas entre o final da tarde e início da noite:

<p align="center">
  <img src="https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/dea49da5-a743-4148-9eea-e44dc9c54ce5">
</p>
<p align="center">
  <img src="https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/61e7e250-6baf-439f-8b59-8bd2ea5787a2">
</p>
<p align="center">
  <img src="https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/901b76bb-f163-49e4-9f87-325ba218cf47">
</p>

Ao adicionar o código que salva no cartão de memória no firmware principal, este passou a reiniciar infinitamente. Fiz alguns ajustes e voltei para uma versão de código anterior as alterações, e o loop infinito continuou. A placa passou a ter um comportamente estranho, após a gravação do meu código, a gravação seguinte apresentava um erro sendo necessário apagar o firmware dentro do microcontrolador e gravar novamente, porém esta alternativa não está mais funcionando pois quando tento apagar o Firmware, recebo a mensagem de erro de que a porta COM não existe. 

<p align="center">
  <img src="https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/ac20ecfb-7f4f-434a-b564-ec2173fb6081">
</p>

Tentei trocar o cabo de gravação, porém os demais não estão funcionando na gravação do ESP32. Tentei gravar um outro firmware qualquer, e o problema não ocorreu. Consegui resolver o problema reiniciando o computador.


# REFERÊNCIAS

<https://randomnerdtutorials.com/esp32-cam-opencv-js-color-detection-tracking/>

<https://github.com/joachimBurket/esp32-opencv>

<https://github.com/lemariva/micropython-camera-driver>

<https://iot.stackexchange.com/questions/6220/esp32-cam-unable-to-use-micropython-and-connect-to-the-esp>

<https://esp32tutorials.com/esp32-static-fixed-ip-address-esp-idf/>

<https://esp32tutorials.com/esp32-web-server-esp-idf/>

<https://github.com/espressif/esp32-camera/tree/master/examples/main>

<https://github.com/espressif/esp-idf/tree/master/examples/storage/spiffs>

<https://firebase.flutter.dev/docs/overview/>

FATECTQ. A INTERNET DAS COISAS: evolução, impactos e benefícios. 2023. Disponível em: <https://revista.fatectq.edu.br/index.php/interfacetecnologica/article/download/538/363>. Acesso em: 26 fev. 2023.

Koyanagi, F. (2019). ESP32 com câmera e reconhecimento facial. Disponível em: <https://www.fernandok.com/2019/04/esp32-com-camera-e-reconhecimento-facial.html>. Acesso em: 28 fev. 2023.

Rayse, K. (2019). Afinal, o que é IOT?. Disponível em: <https://via.ufsc.br/afinal-o-que-e-iot/>. Acesso em: 28 fev. 2023.

CIPELLI, Antonio Marco; MARKUS, Otávio; SANDRINI, Waldir. Teoria e desenvolvimento de projetos de circuitos eletrônicos. 21 ed. São Paulo: Érica, 2005.

MAMEDE FILHO, João. Proteção de equipamentos eletrônicos sensíveis. 2.ed. São Paulo: Érica, 2011.

RAUEN, F. J. Roteiros de investigação científica. Tubarão: UNISUL, 2002.
