# 1 INTRODUÇÃO

## 1.1 Justificativa

Este trabalho se justifica por dar mais controle ao morador sobre a abertura e fechamento de seu portão de casa. Ao final do desenvolvimento, será possível controlar de forma remota via aplicativo de celular, o envio de comandos a um emissor de sinal, além do recebimento de notificações sobre a mudança de estado (aberto ou fechado).

## 1.2 Definição do problema

O conceito de internet das coisas é relativamente novo, criado em 1999 por Kevin Ashton ao criar um sistema com RFID para rastrear um produto em uma cadeia de suprimentos (revista FACTEQ 2023).

Desde então foram desenvolvidos outros produtos para facilitar o dia a dia das pessoas, criando um controle cada vez maior das pessoas sobre os objetos que estão a sua volta. 

Eventualmente saímos de casa e nos deparamos com situações de que podemos ter esquecido o portão aberto, ou de poder liberar o acesso a alguém sem termos que procurar a chave ou controle, ou até mesmo sem estarmos em casa. Assim, surge a seguinte pergunta: “Como criar um sistema que possa controlar à distância os estados (aberto e fechado) de um portão eletrônico comum?”.

## 1.3 Objetivo geral

Desenvolver um sistema de comunicação entre o portão eletrônico e um smartphone, onde uma câmera capta imagens do portão e repassa a um microcontrolador, este por sua vez analisará se o portão não está fechado por completo por um período determinado de tempo, e se comunicará com o smartphone via sinal de wifi onde o usuário irá escolher qual ação deve ser tomada. Este fluxo é ilustrado pelo diagrama de blocos a seguir:
	
![image](https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/bf5ebd5d-215c-4fb8-baf3-ad1f694dfba9)

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

![image](https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/2bce7f96-51e6-4d30-b9a1-274b8e7e873c)

# 5 DESENVOLVIMENTO

## 5.1 Linguagem e IDE de desenvolvimento

Para realizar o projeto, após pesquisa, resolvi optar por comprar o microcontrolador ESP32-CAM que é um modelo amplamente utilizado no mundo inteiro, que possui uma boa documentação, atende aos requisitos de projeto (criar um webserver, tirar fotos e analisar as imagens).
Decidi comprar duas placas iguais (para caso uma delas queimasse durante os testes), e uma placa um pouco mais robusta (ESP32-CAM Wrover).

Com as placas em mãos, iniciei com o plano de utilizar o Micropython, segui alguns tutoriais diferentes e em todos eles eu consegui fazer o upload do firmware, mas ao abrir o programa Thonny, sempre me deparava om a mensagem de que a porta COM estava ocupada. Tentei buscar a solução deste problema por aproximadamente três dias, desisti e segui por um outro caminho, programar em C/C++ utilizando o Visual Studio Code.

Com o Visual Studio Code, consegui rodar alguns exemplos, testei o uso da extensão PlataformIO e também da ESP-IDF, porém diversas vezes me deparava com o problema de do Software travar o carregamento e consequentemente me atrasando no desenvolvimento.

Após conversa com um colega de sala, ele sugeriu o uso da IDE ESP-IDF para desenvolver o firmware. Utilizei de alguns exemplos do Github oficial da Espressif como base para o desenvolvimento do projeto, tive aqui também alguns problemas de travamento, porém consegui resolvê-los com maior rapidez.

## 5.2 Firmware e aplicativo

Iniciei o desenvolvimento do firmware configurando o wifi no microcontrolador, depois configurei a câmera e webserver para visualizar as fotos e transmissão do vídeo. Quando finalizei esta etapa, iniciei o desenvolvimento da comunicação entre o microcontrolador e o smartphone.

Utilizei o Visual Studio Code para desenvolver o aplicativo com o framework Flutter, que utiliza a linguagem DART. Escolhi o Flutter pois me permite desenvolver um único códicgo para ser utilizado nas plataformas Android e IOS. Criei o aplicativo com apenas um botão e que exibe em sua tela uma webview com as imagens geradas pelo microcontrolador. Abaixo está a foto do aplicativo:

![image](https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/a4402aa8-267d-4bf2-bc72-8e66ba76feb0)

Para receber os comandos de abrir/fechar o portão, configurei no aplicativo o serviço de envio de mensagens do Firebase, conforme ilustrado abaixo:

![image](https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/98cb3e4d-4fa9-4493-99a0-1ddcaa0d7192)

Para validar o funcionamento deste serviço, utilizando o softaware Postman simulando o envio de requisição HTTPS ao Firebase, e este por sua vez direciona a mensagem ao aplicativo. A próxima imagem mostra um exemplo desta comunicação.

![image](https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/8870520b-e316-47cd-b078-2b8f8d4a584d)

Após validar o funcionamento da comunicação, foi feito o desenvolvimento desta requisição no microcontrolador. Na sequência, desenvolvi no aplicativo o envio de uma requisição para o microcontrolador no momento que o usuário clicar no botão "abrir/fechar", e no microcontrolador o recebimento desta mensagem. As figuras a seguir ilustram o código do smartphone e a confirmação do recebimento dela no microcontrolador, consequentemente:

![image](https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/b449ed27-722a-4d47-97af-85bca2e961ca)

![image](https://github.com/brunoeduf1/ProjetoIntegrador/assets/69606316/8b656695-8665-4ce1-96fc-8d357f658a01)



# REFERÊNCIAS

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
