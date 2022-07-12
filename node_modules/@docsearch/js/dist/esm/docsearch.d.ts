import type { DocSearchProps as DocSearchComponentProps } from '@docsearch/react';
interface DocSearchProps extends DocSearchComponentProps {
    container: HTMLElement | string;
    environment?: typeof window;
}
export declare function docsearch(props: DocSearchProps): void;
export {};
