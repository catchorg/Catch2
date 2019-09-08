<a id="top"></a>
# Casos de teste e Seções

**Conteúdo**<br>
[Tags](#tags)<br>
[Tag aliases](#tag-aliases)<br>
[Casos de teste no estilo BDD](#casos-de-teste-no-estilo-bdd)<br>
[Casos de teste com tipos parametrizados](#casos-de-teste-com-tipos-parametrizados)<br>
[Casos de teste parametrizados baseados em assinatura](#casos-de-teste-parametrizados-baseados-em-assinatura)<br>

Embora o Catch ofereça suporte total ao estilo tradicional, xUnit, de acessórios baseados em classe que contêm métodos de caso de teste, esse não é o estilo preferido.

Ao invés disso, o Catch fornece um mecanismo poderoso para aninhar seções de casos de teste em um caso de teste. Para uma discussão mais detalhada, dê uma olhada em [tutorial](tutorial.md#test-cases-and-sections).

Casos de teste e Seções aõ muito fáceis de usar na prática:

* **TEST_CASE(** _test name_ \[, _tags_ \] **)**
* **SECTION(** _section name_ **)**

_test name_ e _section name_ são formas livres, citações, strings. Os argumentos _tags_ são opcionais, é uma string conntendo uma ou mais tags dentro de um par de colchetes. Tags são discutidas abaixo. Nomes de testes devem ser únicos dentro de um arquivo executável de testes de Catch.

Para exemplos, veja [Tutorial](tutorial.md#top)

## Tags

Tags permitem um número arbitrário de strings adicionais para serem associadas a um caso de teste. Casos de teste podem ser selecionados (para execução, ou somente para listagem) por tag - ou mesmo por uma combinação que combina várias tags. Na sua forma mais básica eles fornecem uma maneira bastante simples de agrupar muitos testes relacionados.

Por exemplo - dados os seguintes casos de testes:

    TEST_CASE( "A", "[widget]" ) { /* ... */ }
    TEST_CASE( "B", "[widget]" ) { /* ... */ }
    TEST_CASE( "C", "[gadget]" ) { /* ... */ }
    TEST_CASE( "D", "[widget][gadget]" ) { /* ... */ }

A tag ```"[widget]"``` engloba A, B & D. ```"[gadget]"``` seleciona C & D. ```"[widget][gadget]"``` seleciona somente D ```"[widget],[gadget]"``` seleciona todos os casos de teste.

Para mais detalhes sobre a seleção na linha de comandos, veja [documentos da linha de comandos](command-line.md#especificando-quais-testes-executar)


## Tags aliases


## Casos de teste no estilo BDD


## Casos de teste com tipos parametrizados


## Casos de teste parametrizados baseados em assinatura

---

[Home](Readme.md#top)
