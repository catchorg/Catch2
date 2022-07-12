<p align="left"> <img width="675" src="https://raw.githubusercontent.com/willmcpo/body-scroll-lock/master/images/logo.png" alt="Body scroll lock...just works with everything ;-)" /> </p>

## Why BSL?

Enables body scroll locking (for iOS Mobile and Tablet, Android, desktop Safari/Chrome/Firefox) without breaking scrolling of a target element (eg. modal/lightbox/flyouts/nav-menus).

_Features:_

- disables body scroll WITHOUT disabling scroll of a target element
- works on iOS mobile/tablet (!!)
- works on Android
- works on Safari desktop
- works on Chrome/Firefox
- works with vanilla JS and frameworks such as React / Angular / VueJS
- supports nested target elements (eg. a modal that appears on top of a flyout)
- can reserve scrollbar width
- `-webkit-overflow-scrolling: touch` still works

_Aren't the alternative approaches sufficient?_

- the approach `document.body.ontouchmove = (e) => { e.preventDefault(); return false; };` locks the
  body scroll, but ALSO locks the scroll of a target element (eg. modal).
- the approach `overflow: hidden` on the body or html elements doesn't work for all browsers
- the `position: fixed` approach causes the body scroll to reset
- some approaches break inertia/momentum/rubber-band scrolling on iOS

_LIGHT Package Size:_

[![minzip size](https://badgen.net/bundlephobia/minzip/body-scroll-lock?color=orange)](https://badgen.net/bundlephobia/minzip/body-scroll-lock?color=orange)

## Install

    $ yarn add body-scroll-lock

    or

    $ npm install body-scroll-lock

You can also load via a `<script src="lib/bodyScrollLock.js"></script>` tag (refer to the lib folder).

## Usage examples

##### Common JS

```javascript
// 1. Import the functions
const bodyScrollLock = require('body-scroll-lock');
const disableBodyScroll = bodyScrollLock.disableBodyScroll;
const enableBodyScroll = bodyScrollLock.enableBodyScroll;

// 2. Get a target element that you want to persist scrolling for (such as a modal/lightbox/flyout/nav).
// Specifically, the target element is the one we would like to allow scroll on (NOT a parent of that element).
// This is also the element to apply the CSS '-webkit-overflow-scrolling: touch;' if desired.
const targetElement = document.querySelector('#someElementId');

// 3. ...in some event handler after showing the target element...disable body scroll
disableBodyScroll(targetElement);

// 4. ...in some event handler after hiding the target element...
enableBodyScroll(targetElement);
```

##### React/ES6

```javascript
// 1. Import the functions
import { disableBodyScroll, enableBodyScroll, clearAllBodyScrollLocks } from 'body-scroll-lock';

class SomeComponent extends React.Component {
  targetElement = null;

  componentDidMount() {
    // 2. Get a target element that you want to persist scrolling for (such as a modal/lightbox/flyout/nav).
    // Specifically, the target element is the one we would like to allow scroll on (NOT a parent of that element).
    // This is also the element to apply the CSS '-webkit-overflow-scrolling: touch;' if desired.
    this.targetElement = document.querySelector('#targetElementId');
  }

  showTargetElement = () => {
    // ... some logic to show target element

    // 3. Disable body scroll
    disableBodyScroll(this.targetElement);
  };

  hideTargetElement = () => {
    // ... some logic to hide target element

    // 4. Re-enable body scroll
    enableBodyScroll(this.targetElement);
  };

  componentWillUnmount() {
    // 5. Useful if we have called disableBodyScroll for multiple target elements,
    // and we just want a kill-switch to undo all that.
    // OR useful for if the `hideTargetElement()` function got circumvented eg. visitor
    // clicks a link which takes him/her to a different page within the app.
    clearAllBodyScrollLocks();
  }

  render() {
    return <div>some JSX to go here</div>;
  }
}
```

##### React/ES6 with Refs

```javascript
// 1. Import the functions
import { disableBodyScroll, enableBodyScroll, clearAllBodyScrollLocks } from 'body-scroll-lock';

class SomeComponent extends React.Component {
  // 2. Initialise your ref and targetElement here
  targetRef = React.createRef();
  targetElement = null;

  componentDidMount() {
    // 3. Get a target element that you want to persist scrolling for (such as a modal/lightbox/flyout/nav).
    // Specifically, the target element is the one we would like to allow scroll on (NOT a parent of that element).
    // This is also the element to apply the CSS '-webkit-overflow-scrolling: touch;' if desired.
    this.targetElement = this.targetRef.current;
  }

  showTargetElement = () => {
    // ... some logic to show target element

    // 4. Disable body scroll
    disableBodyScroll(this.targetElement);
  };

  hideTargetElement = () => {
    // ... some logic to hide target element

    // 5. Re-enable body scroll
    enableBodyScroll(this.targetElement);
  };

  componentWillUnmount() {
    // 5. Useful if we have called disableBodyScroll for multiple target elements,
    // and we just want a kill-switch to undo all that.
    // OR useful for if the `hideTargetElement()` function got circumvented eg. visitor
    // clicks a link which takes him/her to a different page within the app.
    clearAllBodyScrollLocks();
  }

  render() {
    return (
      // 6. Pass your ref with the reference to the targetElement to SomeOtherComponent
      <SomeOtherComponent ref={this.targetRef}>some JSX to go here</SomeOtherComponent>
    );
  }
}

// 7. SomeOtherComponent needs to be a Class component to receive the ref (unless Hooks - https://reactjs.org/docs/hooks-faq.html#can-i-make-a-ref-to-a-function-component - are used).
class SomeOtherComponent extends React.Component {
  componentDidMount() {
    // Your logic on mount goes here
  }

  // 8. BSL will be applied to div below in SomeOtherComponent and persist scrolling for the container
  render() {
    return <div>some JSX to go here</div>;
  }
}
```


##### Angular

```javascript
import { Component, ElementRef, OnDestroy, ViewChild } from "@angular/core";

// 1. Import the functions
import {
  disableBodyScroll,
  enableBodyScroll,
  clearAllBodyScrollLocks
} from "body-scroll-lock";

@Component({
  selector: "app-scroll-block",
  templateUrl: "./scroll-block.component.html",
  styleUrls: ["./scroll-block.component.css"]
})
export class SomeComponent implements OnDestroy {
  // 2. Get a target element that you want to persist scrolling for (such as a modal/lightbox/flyout/nav).
  // Specifically, the target element is the one we would like to allow scroll on (NOT a parent of that element).
  // This is also the element to apply the CSS '-webkit-overflow-scrolling: touch;' if desired.
  @ViewChild("scrollTarget") scrollTarget: ElementRef;

  showTargetElement() {
    // ... some logic to show target element

    // 3. Disable body scroll
    disableBodyScroll(this.scrollTarget.nativeElement);
  }
  
  hideTargetElement() {
    // ... some logic to hide target element

    // 4. Re-enable body scroll
    enableBodyScroll(this.scrollTarget.nativeElement);
  }

  ngOnDestroy() {
    // 5. Useful if we have called disableBodyScroll for multiple target elements,
    // and we just want a kill-switch to undo all that.
    // OR useful for if the `hideTargetElement()` function got circumvented eg. visitor
    // clicks a link which takes him/her to a different page within the app.
    clearAllBodyScrollLocks();
  }
}

```

##### Vanilla JS

In the html:

```html
<head>
  <script src="some-path-where-you-dump-the-javascript-libraries/lib/bodyScrollLock.js"></script>
</head>
```

Then in the javascript:

```javascript
// 1. Get a target element that you want to persist scrolling for (such as a modal/lightbox/flyout/nav).
// Specifically, the target element is the one we would like to allow scroll on (NOT a parent of that element).
// This is also the element to apply the CSS '-webkit-overflow-scrolling: touch;' if desired.
const targetElement = document.querySelector('#someElementId');

// 2. ...in some event handler after showing the target element...disable body scroll
bodyScrollLock.disableBodyScroll(targetElement);

// 3. ...in some event handler after hiding the target element...
bodyScrollLock.enableBodyScroll(targetElement);

// 4. Useful if we have called disableBodyScroll for multiple target elements,
// and we just want a kill-switch to undo all that.
bodyScrollLock.clearAllBodyScrollLocks();
```

## Demo

Check out the demo, powered by Vercel.

* https://bodyscrolllock.vercel.app for a basic example 
* https://bodyscrolllock-modal.vercel.app for an example with a modal.

## Functions

| Function                  | Arguments                                                      | Return | Description                                                  |
| :------------------------ | :------------------------------------------------------------- | :----: | :----------------------------------------------------------- |
| `disableBodyScroll`       | `targetElement: HTMLElement` <br/>`options: BodyScrollOptions` | `void` | Disables body scroll while enabling scroll on target element |
| `enableBodyScroll`        | `targetElement: HTMLElement`                                   | `void` | Enables body scroll and removing listeners on target element |
| `clearAllBodyScrollLocks` | `null`                                                         | `void` | Clears all scroll locks                                      |

## Options

### reserveScrollBarGap

**optional, default:** false

If the overflow property of the body is set to hidden, the body widens by the width of the scrollbar. This produces an
unpleasant flickering effect, especially on websites with centered content. If the `reserveScrollBarGap` option is set,
this gap is filled by a `padding-right` on the body element. If `disableBodyScroll` is called for the last target element,
or `clearAllBodyScrollLocks` is called, the `padding-right` is automatically reset to the previous value.

```js
import { disableBodyScroll } from 'body-scroll-lock';
import type { BodyScrollOptions } from 'body-scroll-lock';

const options: BodyScrollOptions = {
  reserveScrollBarGap: true,
};

disableBodyScroll(targetElement, options);
```

### allowTouchMove

**optional, default:** undefined

To disable scrolling on iOS, `disableBodyScroll` prevents `touchmove` events.
However, there are cases where you have called `disableBodyScroll` on an
element, but its children still require `touchmove` events to function.

See below for 2 use cases:

##### Simple

```javascript
disableBodyScroll(container, {
  allowTouchMove: el => el.tagName === 'TEXTAREA',
});
```

##### More Complex

Javascript:

```javascript
disableBodyScroll(container, {
  allowTouchMove: el => {
    while (el && el !== document.body) {
      if (el.getAttribute('body-scroll-lock-ignore') !== null) {
        return true;
      }

      el = el.parentElement;
    }
  },
});
```

Html:

```html
<div id="container">
  <div id="scrolling-map" body-scroll-lock-ignore>
    ...
  </div>
</div>
```

## References

https://medium.com/jsdownunder/locking-body-scroll-for-all-devices-22def9615177
https://stackoverflow.com/questions/41594997/ios-10-safari-prevent-scrolling-behind-a-fixed-overlay-and-maintain-scroll-posi

## Changelog

Refer to the [releases](https://github.com/willmcpo/body-scroll-lock/releases) page.
