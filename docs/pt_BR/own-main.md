<a id="top"></a>
# Definindo sua própria função main()

**Conteúdo**<br>
[Deixando o Catch ter total controle sobre os argumentos e configurações](#deixando-o-Catch-ter-total-controle-sobre-os-argumentos-e-configurações)<br>
[Alterando a configuração](#alterando-a-configuração)<br>
[Adicionando suas próprias opções de linha de comandos](#adicionando-suas-próprias-opções-de-linha-de-comandos)<br>
[Detecção de versão](#detecção-de-versão)<br>

A maneira mais fácil de usar o Catch é deixá-lo fornecer a função ```main()``` pra você e manipular as configurações a partir da linha de comandos.

Isso é feito ao escrever ```#define CATCH_CONFIG_MAIN``` antes da inclusão do Catch com ```#include "catch.hpp"``` em **exatamente um** dos seus códigos fontes de testes.

Porém, ás vezes, você precisa escrever sua própria versão da função ```main()```. Você pode fazer isso usando ```#define CATCH_CONFIG_RUNNER``` ao invés do anterior. Agora você está livre pra implementar seu próprio ```main()``` e chamá-lo no Catch manualmente.

Agora você tem muito mais flexibilidade. Abaixo, seguem três dicas para começar:

## Deixando o Catch ter total controle sobre os argumentos e configurações

Se você precisa somente de um código que executa antes e/ou depois do Catch, essa é uma boa opção mais simples.

```c++
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main(int argc, char *argv[]) {
    // global setup...

    int result = Catch::Session().run( argc, argv );

    // global clean-up ...

    return result;
}
```

## Alterando a configuração

Se você ainda quer que o Catch processe a partir da linha de comandos, mas também quer ajustar programaticamente a configuração,você pode fazer isso de duas maneiras:

```c++
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main( int argc, char* argv[] )
{
  Catch::Session session; // Deve existir somente uma instância
 
  // escrever em session.configData() aqui configura padrões
  // esta é a melhor forma de definí-los
    
  int returnCode = session.applyCommandLine( argc, argv );
  if( returnCode != 0 ) // Indicates a command line error
        return returnCode;
 
  // escrever em session.configData() ou session.Config() aqui
  // sobrescreve os argumentos da linha de comandos
  // faça isso somente se você souber do que precisa

  int numFailed = session.run();
  
  // numFailed é fixo em 255 pois algumas unidades usam só os 8 bits inferiores.
  // Este fixador já foi aplicado então é só devolvê-lo aqui
  // Você pode fazer qualquer limpeza de pós-execução aqui
  return numFailed;
}
```

Dê uma olhada nas definições do Config e ConfigData pra ver o que você pode fazer com elas.

Para assumir o controle total da configuração, simplesmente omita a chamada para ```applyCommandLine()```.

## Adicionando suas próprias opções de linha de comandos

O Catch é embarcado com um poderoso interpretador de linha de comandos, veja [Clara](https://github.com/catchorg/Clara).

A partir do Catch (e o Clara 1.0), o Clara permite escrever opções que podem ser compostas e analisadores de argumentos, assim é fácil escrever as próprias opções de linhas de comandos do Catch.

```c++
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main( int argc, char* argv[] )
{
  Catch::Session session; // Deve existir somente uma instância
  
  int height = 0; // Alguma variável que você deseja poder definir por linha de comando
  
  // Crie um novo interpretador no topo do Catch
  using namespace Catch::clara;
  auto cli 
    = session.cli() // Get Catch's composite command line parser
    | Opt( height, "height" ) // bind variable to a new option, with a hint string
        ["-g"]["--height"]    // the option names it will respond to
        ("how high?");        // description string for the help output
        
  // Passe o novo componente de volta para o Catch para que ele possa usá-lo
  session.cli( cli ); 
  
  // Permita que o Catch (usando o Clara) interprete a linha de comandos
  int returnCode = session.applyCommandLine( argc, argv );
  if( returnCode != 0 ) // Indicates a command line error
      return returnCode;

  // se definido na linha de comandos, então 'height' já tem o valor passado neste ponto
  if( height > 0 )
      std::cout << "height: " << height << std::endl;

  return session.run();
}
```

Veja a [documentação do Clara](https://github.com/philsquared/Clara/blob/master/README.md) para mais detalhes.

## Detecção de versão

O Catch fornece três macros que definem a versão do cabeçalho individual (single header),

* `CATCH_VERSION_MAJOR`
* `CATCH_VERSION_MINOR`
* `CATCH_VERSION_PATCH`

essas macros compõem um único número que corresponde ao formato de versão apropriado. Por exemplo, para uma versão v2.3.4 as macros são expandidas em `2`, `3` e `4` respectivamente.

---

[Home](Readme.md#top)
