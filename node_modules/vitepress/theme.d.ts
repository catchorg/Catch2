// so that users can do `import DefaultTheme from 'vitepress/theme'`
import { ComponentOptions } from 'vue'

export const VPHomeHero = ComponentOptions
export const VPHomeFeatures = ComponentOptions
export const VPHomeSponsors = ComponentOptions
export const VPDocAsideSponsors = ComponentOptions
export const VPTeamPage = ComponentOptions
export const VPTeamPageTitle = ComponentOptions
export const VPTeamPageSection = ComponentOptions
export const VPTeamMembers = ComponentOptions

declare const theme: {
  Layout: ComponentOptions
  NotFound: ComponentOptions
}

export default theme
