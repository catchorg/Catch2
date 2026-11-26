library(XML)
library(ggplot2)

args <- commandArgs(trailingOnly = TRUE)
data <- xmlParse(args[1])

for (test_case in data["//TestCase"]) {
  test_case_name <- xpathSApply(test_case, "./@name", paste)
  results <- xpathSApply(test_case, ".//BenchmarkResults")
  samples <- xpathSApply(test_case, ".//sample", xmlValue)
  counts <- xpathSApply(test_case, ".//BenchmarkResults", function(r) {
    xmlGetAttr(r, "samples", default = 0, converter = as.integer)
  })
  names <- xpathSApply(test_case, ".//BenchmarkResults/@name", paste)

  if (length(names) == 0 || sum(counts) == 0) next
  names <- rep(names, counts)

  df <- data.frame(
    methods = names,
    samples = as.numeric(samples)
  )

  p <- ggplot(df, aes(x = methods, y = samples, fill = methods)) +
    geom_boxplot(size = 0.5, staplewidth = 0.5) +
    theme(axis.text.x = element_text(angle = -45, vjust = 1, hjust = 0)) +
    labs(x = "Benchmark", y = "Time [ns]", title = test_case_name, fill = "Benchmark") +
    scale_y_continuous(limits = c(0, NA))
  print(p)
  test_case_name |>
    gsub(pattern = "\\s", replacement = "_") |>
    paste(".png", sep = "") |>
    ggsave()
}
