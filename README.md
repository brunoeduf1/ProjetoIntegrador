BRUNO EDUARDO FERREIRA
DESENVOLVIMENTO DE UM SISTEMA IOT PARA
CONTROLE DE PORTÃO ELETRÔNICO
FLORIANÓPOLIS, 2023
INSTITUTO FEDERAL DE EDUCAÇÃO, CIÊNCIA E TECNOLOGIA DE SANTA
CATARINA
DEPARTAMENTO ACADÊMICO DE ELETRÔNICA
CURSO SUPERIOR DE ENGENHARIA ELETRÔNICA
BRUNO EDUARDO FERREIRA
DESENVOLVIMENTO DE UM SISTEMA IOT PARA
CONTROLE DE PORTÃO ELETRÔNICO
Trabalho acadêmico submetido ao
Instituto Federal de Educação, Ciência e
Tecnologia de Santa Catarina para
aprovação na unidade curricular de
Projeto Integrador III.
Professores orientadores: Mst. Daniel
Lohmann e Dr. Robinson Pizzio.
FLORIANÓPOLIS, 2023
LISTA DE SIGLAS
CI – Circuito Integrado
IFSC - Instituto Federal de Educação, Ciência e Tecnologia de Santa Catarina
IOT – Internet of things (internet das coisas)
MCC - Microcontrolador
SUMÁRIO
1 INTRODUÇÃO......................................................................................................................5
1.1 Justificativa...........................................................................................................................5
1.2 Definição do problema..........................................................................................................5
1.3 Objetivo geral........................................................................................................................5
1.4 Objetivos específicos............................................................................................................7
2 REVISÃO DE LITERATURA.............................................................................................8
2.1 Conceito de internet das coisas.............................................................................................8
2.2 Semicondutores, condutores e isolantes................................................................................8
2.3 Fenômenos transientes..........................................................................................................9
2.4 Blindagem eletromagnética...................................................................................................9
3 METODOLOGIA................................................................................................................10
REFERÊNCIAS......................................................................................................................11
1 INTRODUÇÃO
1.1 Justificativa
Este trabalho se justifica por dar mais controle ao morador sobre a abertura e
fechamento de seu portão de casa. Ao final do desenvolvimento, será possível
controlar de forma remota via aplicativo de celular, o envio de comandos a um
emissor de sinal, além do recebimento de notificações sobre a mudança de estado
(aberto ou fechado).
1.2 Definição do problema
O conceito de internet das coisas é relativamente novo, criado em 1999 por
Kevin Ashton ao criar um sistema com RFID para rastrear um produto em uma
cadeia de suprimentos (revista FACTEQ 2023).
Desde então foram desenvolvidos outros produtos para facilitar o dia a dia
das pessoas, criando um controle cada vez maior das pessoas sobre os objetos que
estão a sua volta.
Eventualmente saímos de casa e nos deparamos com situações de que
podemos ter esquecido o portão aberto, ou de poder liberar o acesso a alguém sem
termos que procurar a chave ou controle, ou até mesmo sem estarmos em casa.
Assim, surge a seguinte pergunta: “Como criar um sistema que possa controlar à
distância os estados (aberto e fechado) de um portão eletrônico comum?”.
1.3 Objetivo geral
Desenvolver um sistema de comunicação entre o portão eletrônico e um
smartphone, onde uma câmera capta imagens do portão e repassa a um
microcontrolador, este por sua vez analisará se o portão não está fechado por
completo por um período determinado de tempo, e se comunicará com o
smartphone via sinal de wifi onde o usuário irá escolher qual ação deve ser tomada.
Este fluxo é ilustrado pelo diagrama de blocos a seguir:
Figura 1: Diagrama de blocos
Koyanagi (2019), desenvolveu um projeto utilizando ESP32-CAM para
reconhecimento facial, mostrando ser possível utilizar as imagens capturadas
através da câmera OV2640 / 2 mp para identificar pessoas através de seus rostos, e
este mesmo princípio pode ser utilizado para identificar as situações do portão
eletrônico.
1.4 Objetivos específicos
Este trabalho tem os seguintes objetivos específicos:
a) Definir os componentes;
b) Comprar os componentes;
c) Montar o circuito;
d) Desenvolver o firmware;
e) Desenvolver o aplicativo;
f) Fabricar a placa de circuito.
2 REVISÃO DE LITERATURA
Neste capítulo será feita abordagem dos conceitos relacionados ao tema para
fundamentar a análise com teorias elaboradas por diversos autores.
2.1 Conceito de internet das coisas
Segundo Kiane (2019), com o aumento da disponibilidade de internet de
banda larga e a incorporação de sensores em dispositivos com conexão Wi-Fi,
houve uma redução no custo dessas tecnologias, levando à popularização de
smartphones e criando um cenário favorável para a Internet das Coisas (IoT). A IoT
consiste em conectar virtualmente qualquer dispositivo à internet, incluindo
componentes de máquinas, formando uma rede de "coisas" conectadas, incluindo
pessoas, permitindo relacionamentos entre pessoas, coisas e pessoas-coisas.
2.2 Semicondutores, condutores e isolantes
Semicondutores são materiais onde sua resistência está entre a dos isolantes
e dos condutores, sendo os de uso mais comum o germânio e o silício (CIPELLI;
MARKUS; SANDRINI, 2005).
Cipelli, Markus e Sandrini (2005, p. 4) afirmam que
“Os condutores são elementos que possuem elétrons livres em grande
quantidade e fracamente ligados ao núcleo, sendo que, sob a ação de uma
diferença de potencial, passam a ser locomover no interior do material
condutor. Quanto maior o número de elétrons livres, maior será o fluxo de
corrente e, consequentemente, maior será sua condutividade.”
Nos isolantes, os elétrons encontram-se fortemente ligados ao núcleo, e
mesmo aquecendo-os, pouca quantidade destes tornam-se livres, o que dificulta o
fluxo de corrente em uma diferença de potencial (CIPELLI; MARKUS; SANDRINI,
2005).
2.3 Fenômenos transientes
Fenômenos transientes “São aqueles gerados por fontes internas e externas
ao sistema supridor, com diferentes formas de onda e de variada intensidade, que
afetam o funcionamento dos equipamentos de tecnologia da informação [...]”. Esses
fenômenos podem causar danos permanentes (MAMEDE FILHO, 2010, p. 168).
2.4 Blindagem eletromagnética
Blindagem eletromagnética significa reduzir os campos eletromagnéticos que
atuam sobre uma instalação e/ou sobre seus componentes. A blindagem é
entendida como uma barreira de metal entre uma fonte emissora de ondas
eletromagnéticas, e um ambiente, equipamento ou algo que queira imunizar contra
interferências que possam afetar negativamente o desempenho dos aparelhos
(MAMEDE FILHO, 2010).
3 METODOLOGIA
Para avaliação do tema deste trabalho, serão utilizados procedimentos
metodológicos que auxiliarão na forma de pesquisa e documentação para atingir os
objetivos propostos do estudo em questão.
Quanto à natureza, esta pesquisa terá caráter de pesquisa aplicada, por ter
como objetivo o desenvolvimento da pesquisa pela prática, visando solucionar
problemas específicos, e envolvendo interesses e verdades locais.
A pesquisa deste trabalho por ter embasamento na realidade de uma
organização, na busca pela compreensão do fenômeno particular e no envolvimento
frequente de pesquisas de campo, será considerada qualitativa (RAUEN, 2002).
Em relação aos objetivos, a pesquisa será exploratória por tornar o problema
explícito, por envolver o levantamento de dados bibliográficos, por buscar
conhecimento com pessoas que já vivenciaram experiências práticas com o
problema em questão, e por analisar problemas e soluções semelhantes que
auxiliarão na compreensão deste problema.
Por ser elaborada com a utilização de material publicado, seja em livros,
internet e/ou artigos, esta pesquisa será considerada como bibliográfica.
REFERÊNCIAS
FATECTQ. A INTERNET DAS COISAS: evolução, impactos e benefícios. 2023.
Disponível em: <https://revista.fatectq.edu.br/index.php/interfacetecnologica/article/
download/538/363>. Acesso em: 26 fev. 2023.
Koyanagi, F. (2019). ESP32 com câmera e reconhecimento facial. Disponível em:
<https://www.fernandok.com/2019/04/esp32-com-camera-e-reconhecimentofacial.
html>. Acesso em: 28 fev. 2023.
Rayse, K. (2019). Afinal, o que é IOT?. Disponível em: <https://via.ufsc.br/afinal-oque-
e-iot/>. Acesso em: 28 fev. 2023.
CIPELLI, Antonio Marco; MARKUS, Otávio; SANDRINI, Waldir. Teoria e
desenvolvimento de projetos de circuitos eletrônicos. 21 ed. São Paulo: Érica,
2005.
MAMEDE FILHO, João. Proteção de equipamentos eletrônicos sensíveis. 2.ed.
São Paulo: Érica, 2011.
RAUEN, F. J. Roteiros de investigação científica. Tubarão: UNISUL, 2002.