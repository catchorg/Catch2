<a id="top"></a>
# Tutorial

**Conteúdo**<br>
[Obtendo o Catch2](#getting-catch2)<br>
[Onde por isto?](#where-to-put-it)<br>
[Escrevendo testes](#writing-tests)<br>
[Casos de testes e seções](#test-cases-and-sections)<br>
[Estilo BDD](#bdd-style)<br>
[Aumentando em escala](#scaling-up)<br>
[Tipo de casos de teste parametrizados](#type-parametrised-test-cases)<br>
[Próximos passos](#next-steps)<br>

## Obtendo o Catch2

A maneira mais fácil de obter o Catch2 é baixar a versão mais recente do [single header version](https://raw.githubusercontent.com/catchorg/Catch2/master/single_include/catch2/catch.hpp) (header versão única). O header versão única é  gerado pela junção de um conjunto de headers induviduais mas ainda é somente o código fonte normal em um arquivo de cabeçalho.

Há meios alternativos de obter o Catch2 que inclui usar o seu sistema de gerenciamento de pacotes ou instalar o mesmo usando [este pacote CMake](cmake-integration.md#installing-catch2-from-git-repository).

O código fonte completo do Catch2, incluindo os projetos de testes, documentação, e outras coisas, está hospedado no Github. [http://catch-lib.net](http://catch-lib.net) redirecionará você para lá.


## Onde por isto?

Catch2 é somente um cabeçalho. Tudo o que você precisa fazer é colocar o arquivo em algum lugar acessível para o seu projeto - em algum lugar central você pode definir o caminho de busca do cabeçalho para encontrar ou na própria árvore do seu projeto! Essa é uma boa opção, particularmente para projetos Open-Source que desejam usar o Catch2 em sua suíte de testes. Veja [esse post de blog para mais informações sobre isso](https://levelofindirection.com/blog/unit-testing-in-cpp-and-objective-c-just-got-ridiculously-easier-still.html).

O resto desse tutorial irá assumir que o cabeçalho de inclusão única do Catch2 (ou o diretório de inclusão) está disponível não qualificado - mas você pode precisar adicionar um prefixo com um nome de pasta se for preciso.

_Se você instalou o Catch2 usando o seu sistema de gerenciamento de pacotes, ou pacote CMake, 
você deve incluir o cabeçalho como `#include <catch2/catch.hpp>`_

## Escrevendo testes

Vamos começar com um exemplo muito simples ([code](../examples/010-TestCase.cpp)). Digamos que você tenha escrito uma função para calcular os fatoriais e agora você quer testá-la (vamos deixar o TDD de lado por enquanto).

```c++
unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}
```

Para manter as coisas simples nós iremos colocar tudo em um só arquivo (<a href="#scaling-up">Veja mais tarde, como estruturar seus arquivos de testes</a>).

```c++
#define CATCH_CONFIG_MAIN  // Essa macro diz ao Catch para fornecer um main() - ele só faz isso em um arquivo .cpp
#include "catch.hpp"

unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}

TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}
```

Isso irá compilar para um executável completo que responde aos [argumentos na linha de comandos](command-line.md#top). Se você somente executá-lo sem nenhum argumento, ele executa todos os casos de testes (nesse caso só existe um), reporta qualquer falha, reporta um sumário de quantos testes passaram e/ou falharam e retorna o número de testes falhos (útil se você quer somente uma resposta _sim_ ou _não_ para "Funcionou?").

Se você rodar isso como está escrito ele passará. Tudo bom. Certo?
Bem, ainda há um bug aqui. Na verdade, a primeira versão desse tutorial que eu postei aqui genuinamente tinha um bug. Então não é completamente inventado (graças a Daryle Walker (```@CTMacUser```) po apontar isso).

Qual o bug? Bom, qual é o fatorial de 0?
[O fatorial de zero é um](http://mathforum.org/library/drmath/view/57128.html) - que é somennte uma dessas coisa que você tem que saber (e lembrar!).

Vamos adicionar isso ao caso de teste:

```c++
TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE( Factorial(0) == 1 );
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}
```

Agora nós obtemos uma falha - algo como:

```
Example.cpp:9: FAILED:
  REQUIRE( Factorial(0) == 1 )
with expansion:
  0 == 1
```

Note que nós obtemos o valor atual do retorno de ```Factorial(0)``` mostrado (0) - apesar de termos usado uma expressão natural como o operador de `==`. Isso nos permite ver exatamente qual é o problema.

Vamos alterar a função Factorial para:

```c++
unsigned int Factorial( unsigned int number ) {
  return number > 1 ? Factorial(number-1)*number : 1;
}
```

Agora todos os testes passam.

É claro que existem ainda muitas questões para lidar. Por exemplo, nós teríamos problemas quando o valor de retorno ultrapassar o valor máximo suportado por um `unsigned int`. Com fatoriais que podem acontecer muito rapidamente. Você pode querer adicionar para esse casos e decidir como lidar com eles. Vamos parar de fazer isso aqui.

### O que fizemos aqui?

Embora este tenha sido um teste simples, foi suficiente para demonstrar algumas coisas sobre como o Catch é usado. Vamos pensar um pouco antes de prosseguirmos.

1. Tudo o que fizemos foi definir (`#define`) um identificador e incluir (`#include`) um cabeçalho e conseguimos tudo - até mesmo uma implementação de `main()` capaz de [reponder aos argumentos da linha de comando](command-line.md#top). Você só pode usar esse `#define` em um arquivo de implementação, por (esperançosamente) razões óbvias. Uma vez que você tenha mais de um arquivo com testes unitários, você apenas incluirá `#include "catch.hpp"` e rodará. Geralmente é uma boa idéia ter um arquivo de implementação dedicado que tenha `#define CATCH_CONFIG_MAIN` e `#include "catch.hpp"`. Você também pode fornecer sua própria implementação do principal e conduzir o Catch sozinho (consulte [Fornecendo seu próprio main()](own-main.md#top)).

2. Introduzimos casos de teste com a macro `TEST_CASE`. Essa macro usa um ou dois argumentos - um nome de teste de forma livre e, opcionalmente, uma ou mais marcas (para mais, consulte <a href="#test-cases-and-sections">Casos de testes e seções</a>. O nome do teste deve ser exclusivo. Você pode executar conjuntos de testes especificando um nome de teste curinga ou uma expressão de tag. Consulte os [documentos da linha de comandos](command-line.md#top) para obter mais informações sobre como executar testes.

3. Os argumentos _name_ e _tags_ são apenas strings. Não tivemos que declarar uma função ou método - ou registrar explicitamente o caso de teste em qualquer lugar. Nos bastidores, uma função com um nome gerado é definida para você e registrada automaticamente usando classes de registro estáticas. Ao abstrair o nome da função, podemos nomear nossos testes sem as restrições dos nomes dos identificadores.

4. Escrevemos nossas _assertions_ de testes individuais usando a macro `REQUIRE`. Em vez de uma macro separada para cada tipo de condição, expressamos a condição naturalmente usando a sintaxe C/C++. Nos bastidores, um conjunto simples de modelos de expressão captura o lado esquerdo e o lado direito da expressão para que possamos exibir os valores em nosso relatório de teste. Como veremos mais adiante, existem outras macros de asserção - mas por causa dessa técnica, o número delas é drasticamente reduzido.


<a id="test-cases-and-sections"></a>
## Casos de Testes e Seções

Muitos frameworks de testes têm um mecanismo de fixação baseado em classes. Isto é, os casos de testes mapeiam para métodos e configurações e desmontagem em comum são realizadas nos métodos `setup()` e `teardown()` respectivamente (ou construtor e destrutor em liguagens, como C++, que suportam desmontagem determinística).

Embora o Catch suporte totalmente essa maneira de trabalhar, há alguns problemas com a abordagem. Em particular, a maneira como seu código deve ser dividido e a granularidade contundente dele podem causar problemas. Você só pode ter um par setup/teardown através de um conjunto de métodos, mas às vezes você quer um setup ligeiramente diferente em cada método, ou você pode até querer vários níveis de setup (um conceito que esclareceremos mais adiante neste tutorial). Foram <a href="http://jamesnewkirk.typepad.com/posts/2007/09/why-you-should-.html">problemas como esse</a> que levou James Newkirk, que liderou a equipe que construiu o NUnit, a começar do zero e <a href="http://jamesnewkirk.typepad.com/posts/2007/09/announcing-xuni.html">criar o xUnit</a>).

O Catch adota uma abordagem diferente (tanto para o NUnit quanto para o xUnit), que é um ajuste mais natural para C ++ e a família de linguagens C. Isto é melhor explicado através de um exemplo ([código](../examples/100-Fix-Section.cpp)):

```c++
TEST_CASE( "vectors can be sized and resized", "[vector]" ) {

    std::vector<int> v( 5 );

    REQUIRE( v.size() == 5 );
    REQUIRE( v.capacity() >= 5 );

    SECTION( "resizing bigger changes size and capacity" ) {
        v.resize( 10 );

        REQUIRE( v.size() == 10 );
        REQUIRE( v.capacity() >= 10 );
    }
    SECTION( "resizing smaller changes size but not capacity" ) {
        v.resize( 0 );

        REQUIRE( v.size() == 0 );
        REQUIRE( v.capacity() >= 5 );
    }
    SECTION( "reserving bigger changes capacity but not size" ) {
        v.reserve( 10 );

        REQUIRE( v.size() == 5 );
        REQUIRE( v.capacity() >= 10 );
    }
    SECTION( "reserving smaller does not change size or capacity" ) {
        v.reserve( 0 );

        REQUIRE( v.size() == 5 );
        REQUIRE( v.capacity() >= 5 );
    }
}
```

Para cada ```SECTION``` o ```TEST_CASE``` é executado do início - 
então como nós entramos em cada seção sabemos que a capacidade é 5. Nós reforçamos esses requisitos com os `REQUIRE`s no nível superior para que possamos ter confiança nele. Isso funciona porque a macro `SECTION` contém uma declaração `if` que chama de volta para Catch para ver se a seção deve ser executada. Uma seção de folha é executada em cada execução através de um `TEST_CASE`. As outras seções são ignoradas. A próxima vez na próxima seção será executada, e assim por diante, até que nenhuma nova seção seja encontrada.

Até aí tudo bem - isso já é uma melhoria na abordagem de configuração / desmontagem porque agora vemos nosso código de configuração em linha e usamos a pilha.

O poder das seções realmente mostra, no entanto, quando precisamos executar uma sequência de operações verificadas. Continuando o exemplo do vector, podemos querer verificar que a tentativa de reservar uma capacidade menor que a capacidade atual do vetor não muda nada. Podemos fazer isso, naturalmente, assim:

```c++
    SECTION( "reserving bigger changes capacity but not size" ) {
        v.reserve( 10 );

        REQUIRE( v.size() == 5 );
        REQUIRE( v.capacity() >= 10 );

        SECTION( "reserving smaller again does not change capacity" ) {
            v.reserve( 7 );

            REQUIRE( v.capacity() >= 10 );
        }
    }
```

As seções podem ser aninhadas em uma profundidade arbitrária (limitada apenas pelo tamanho da pilha). Cada seção de folha (ou seja, uma seção que não contém seções aninhadas) será executada exatamente uma vez, em um caminho separado de execução de qualquer outra seção de folha (assim nenhuma seção de folha pode interferir em outra). Uma falha em uma seção pai impedirá a execução de seções aninhadas - mas essa é a ideia.

## Estilo BDD

Se você nomear seus casos de teste e seções apropriadamente, poderá obter uma estrutura de especificação no estilo do BDD. Isso se tornou uma maneira tão útil de trabalhar que o suporte de primeira classe foi adicionado ao Catch. Cenários podem ser especificados usando as macros `SCENARIO`, `GIVEN`, `WHEN` e `THEN`, que mapeiam para `TEST_CASE`s e `SECTION`s, respectivamente. Para mais detalhes veja [Casos de Testes e Seções](test-cases-and-sections.md#top)

O vector exemplificado pode ser ajustado para usar essas macros como ([código exemplo](../examples/120-Bdd-ScenarioGivenWhenThen.cpp)):

```c++
SCENARIO( "vectors can be sized and resized", "[vector]" ) {

    GIVEN( "A vector with some items" ) {
        std::vector<int> v( 5 );

        REQUIRE( v.size() == 5 );
        REQUIRE( v.capacity() >= 5 );

        WHEN( "the size is increased" ) {
            v.resize( 10 );

            THEN( "the size and capacity change" ) {
                REQUIRE( v.size() == 10 );
                REQUIRE( v.capacity() >= 10 );
            }
        }
        WHEN( "the size is reduced" ) {
            v.resize( 0 );

            THEN( "the size changes but not capacity" ) {
                REQUIRE( v.size() == 0 );
                REQUIRE( v.capacity() >= 5 );
            }
        }
        WHEN( "more capacity is reserved" ) {
            v.reserve( 10 );

            THEN( "the capacity changes but not the size" ) {
                REQUIRE( v.size() == 5 );
                REQUIRE( v.capacity() >= 10 );
            }
        }
        WHEN( "less capacity is reserved" ) {
            v.reserve( 0 );

            THEN( "neither size nor capacity are changed" ) {
                REQUIRE( v.size() == 5 );
                REQUIRE( v.capacity() >= 5 );
            }
        }
    }
}
```

Convenientemente, esses testes serão relatados da seguinte maneira quando executados:

```
Scenario: vectors can be sized and resized
     Given: A vector with some items
      When: more capacity is reserved
      Then: the capacity changes but not the size
```

<a id="scaling-up"></a>
## Aumentando em escala

Para manter o tutorial simples, colocamos todo o nosso código em um único arquivo. Isso é bom para começar - e faz pular em Catch ainda mais rápido e mais fácil. Como você escreve mais testes do mundo real, essa não é a melhor abordagem.

O requisito é que o seguinte bloco de código ([ou equivalente](own-main.md#top)):

```c++
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
```

aparece em _exatamente um_ arquivo de origem. Use quantos arquivos cpp adicionais (ou o que quer que você chame seus arquivos de implementação) como você precisa para seus testes, particionados, no entanto, faz mais sentido para o seu jeito de trabalhar. Cada arquivo adicional só precisa de `#include " catch.hpp "` - não repita o `#define`!

De fato isso é utilmente uma boa ideia para colocar o bloco com o `#define` [em seu próprio arquivo fonte](slow-compiles.md#top) (código exemplo [main](../examples/020-TestCase-1.cpp), [tests](../examples/020-TestCase-2.cpp)).

Não escrva seus testes em arquivos de cabeçalho!


## Casos de Testes de tipos parametrizados

Casos de testes no Catch2 também podem ser parametrizados por tipo, através das macros
`TEMPLATE_TEST_CASE` e `TEMPLATE_PRODUCT_TEST_CASE`,
que se comportam da mesma forma que a macro `TEST_CASE`, mas são executados para cada tipo ou combinação de tipos.

Para mais detalhes, veja nossa documentação em [Casos de Testes e Seções](test-cases-and-sections.md#type-parametrised-test-cases).


## Próximos passos

Esta foi uma breve introdução para você começar a trabalhar com o Catch e apontar algumas das principais diferenças entre o Catch e outros frameworks com os quais você já pode estar familiarizado. Isso fará com que você vá muito longe e agora você está em condições de mergulhar e escrever alguns testes.

É claro que há mais para aprender - a maioria dos quais você deve ser capaz de fazer o erro de paginação à medida que avança. Por favor, veja a sempre crescente [Seção de referência](Readme.md#top) para saber o que está disponível.

---

[Home](Readme.md#top)
