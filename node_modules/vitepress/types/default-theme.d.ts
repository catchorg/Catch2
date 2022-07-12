export namespace DefaultTheme {
  export interface Config {
    /**
     * The logo file of the site.
     *
     * @example '/logo.svg'
     */
    logo?: ThemeableImage

    /**
     * Custom site title in navbar. If the value is undefined,
     * `config.title` will be used.
     */
    siteTitle?: string | false

    /**
     * Custom outline title in the aside component.
     *
     * @default 'On this page'
     */
    outlineTitle?: string

    /**
     * The nav items.
     */
    nav?: NavItem[]

    /**
     * The sidebar items.
     */
    sidebar?: Sidebar

    /**
     * Info for the edit link. If it's undefined, the edit link feature will
     * be disabled.
     */
    editLink?: EditLink

    /**
     * Set custom last updated text.
     *
     * @default 'Last updated'
     */
    lastUpdatedText?: string

    /**
     * The social links to be displayed at the end of the nav bar. Perfect for
     * placing links to social services such as GitHub, Twitter, Facebook, etc.
     */
    socialLinks?: SocialLink[]

    /**
     * The footer configuration.
     */
    footer?: Footer

    /**
     * Adds locale menu to the nav. This option should be used when you have
     * your translated sites outside of the project.
     */
    localeLinks?: LocaleLinks

    /**
     * The algolia options. Leave it undefined to disable the search feature.
     */
    algolia?: AlgoliaSearchOptions

    /**
     * The carbon ads options. Leave it undefined to disable the ads feature.
     */
    carbonAds?: CarbonAdsOptions
  }

  // nav -----------------------------------------------------------------------

  export type NavItem = NavItemWithLink | NavItemWithChildren

  export type NavItemWithLink = {
    text: string
    link: string

    /**
     * `activeMatch` is expected to be a regex string. We can't use actual
     * RegExp object here because it isn't serializable
     */
    activeMatch?: string
  }

  export type NavItemChildren = {
    text?: string
    items: NavItemWithLink[]
  }

  export interface NavItemWithChildren {
    text?: string
    items: (NavItemChildren | NavItemWithLink)[]

    /**
     * `activeMatch` is expected to be a regex string. We can't use actual
     * RegExp object here because it isn't serializable
     */
    activeMatch?: string
  }

  // image -----------------------------------------------------------------------

  export type ThemeableImage = Image | { light: Image; dark: Image }
  export type Image = string | { src: string; alt?: string }

  // sidebar -------------------------------------------------------------------

  export type Sidebar = SidebarGroup[] | SidebarMulti

  export interface SidebarMulti {
    [path: string]: SidebarGroup[]
  }

  export interface SidebarGroup {
    text?: string
    items: SidebarItem[]

    /**
     * If `true`, toggle button is shown.
     *
     * @default false
     */
    collapsible?: boolean

    /**
     * If `true`, collapsible group is collapsed by default.
     *
     * @default false
     */
    collapsed?: boolean
  }

  export interface SidebarItem {
    text: string
    link: string
  }

  // edit link -----------------------------------------------------------------

  export interface EditLink {
    /**
     * Pattern for edit link.
     *
     * @example 'https://github.com/vuejs/vitepress/edit/main/docs/:path'
     */
    pattern: string

    /**
     * Custom text for edit link.
     *
     * @default 'Edit this page'
     */
    text?: string
  }

  // social link ---------------------------------------------------------------

  export interface SocialLink {
    icon: SocialLinkIcon
    link: string
  }

  export type SocialLinkIcon =
    | 'discord'
    | 'facebook'
    | 'github'
    | 'instagram'
    | 'linkedin'
    | 'slack'
    | 'twitter'
    | 'youtube'

  // footer --------------------------------------------------------------------

  export interface Footer {
    message?: string
    copyright?: string
  }

  // team ----------------------------------------------------------------------

  export interface TeamMember {
    avatar: string
    name: string
    title?: string
    org?: string
    orgLink?: string
    desc?: string
    links?: SocialLink[]
    sponsor?: string
  }

  // locales -------------------------------------------------------------------

  export interface LocaleLinks {
    text: string
    items: LocaleLink[]
  }

  export interface LocaleLink {
    text: string
    link: string
  }

  // algolia ------------------------------------------------------------------

  /**
   * The Algolia search options. Partially copied from
   * `@docsearch/react/dist/esm/DocSearch.d.ts`
   */
  export interface AlgoliaSearchOptions {
    appId: string
    apiKey: string
    indexName: string
    placeholder?: string
    searchParameters?: any
    disableUserPersonalization?: boolean
    initialQuery?: string
  }

  // carbon ads ----------------------------------------------------------------

  export interface CarbonAdsOptions {
    code: string
    placement: string
  }
}
