// types shared between server and client

export { DefaultTheme } from './default-theme'

export interface PageData {
  relativePath: string
  title: string
  titleTemplate?: string | boolean
  description: string
  headers: Header[]
  frontmatter: Record<string, any>
  lastUpdated?: number
}

export interface Header {
  level: number
  title: string
  slug: string
}

export interface SiteData<ThemeConfig = any> {
  base: string

  /**
   * Language of the site as it should be set on the `html` element.
   *
   * @example `en-US`, `zh-CN`
   */
  lang: string

  title: string
  titleTemplate?: string | boolean
  description: string
  head: HeadConfig[]
  appearance: boolean
  themeConfig: ThemeConfig
  scrollOffset: number | string
  locales: Record<string, LocaleConfig>

  /**
   * Available locales for the site when it has defined `locales` in its
   * `themeConfig`. This object is otherwise empty. Keys are paths like `/` or
   * `/zh/`.
   */
  langs: Record<
    string,
    {
      /**
       * Lang attribute as set on the `<html>` element.
       * @example `en-US`, `zh-CN`
       */
      lang: string
      /**
       * Label to display in the language menu.
       * @example `English`, `简体中文`
       */
      label: string
    }
  >
}

export type HeadConfig =
  | [string, Record<string, string>]
  | [string, Record<string, string>, string]

export interface LocaleConfig {
  lang: string
  title?: string
  titleTemplate?: string | boolean
  description?: string
  head?: HeadConfig[]
  label?: string
  selectText?: string
}

export interface PageDataPayload {
  path: string
  pageData: PageData
}
