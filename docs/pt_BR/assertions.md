<a id="top"></a>
# Macros de Asserções

**Conteúdo**<br>
[Expressões naturais](#expressões-naturais)
[Exceções](#exceções)
[Matcher de expressões](#matcher-de-expressões)
[Segurança de thread](#segurança-de-thread)
[Expressões com vírgulas](#expressões-com-vígulas)

Muitos frameworks de testes têm uma grande coleção de macros de asserção para capturar todas as formas condicionais possíveis (```_EQUAL```, ```_NOTEQUALS```, ```_GREATER_THAN``` etc).

Catch2 é diferente. Porque ele decompõe expressões condicionais naturais do Estilo-C, muitas dessas formas são reduzidas a uma ou duas que você irá usar todo o tempo. Dito isso, também há um rico conjunto de macros auxiliares. Nós vamos descrever todas elas aqui.

Muitas dessas macros são de duas formas:

## Expressões Naturais

A família de macros ```REQUIRE``` testa uma expressão e aborta o caso de teste se a mesma é falsa.

A família ```CHECK``` são equivalentes, mas a execução continua no mesmo caso de teste mesmo depois que a asserção falha. Isso é muito útil se você possui uma série de asserções essencialmente ortogonais e pode ser usado paar ver todos o resultado mesmo que a primeira asserção do caso de teste falhe.

* **REQUIRE(** _expression_ **)** e
* **CHECK(** _expression_ **)**

---

[Home](Readme.md#top)
