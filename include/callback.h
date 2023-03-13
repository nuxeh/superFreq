#ifndef __SUPER_FREQ_CALLBACK_H__
#define __SUPER_FREQ_CALLBACK_H__

enum CallbackEvent : uint8_t {
  Started = 0,
  Stopped,
  Asserted,
  Deasserted,
};

struct superFreqCallbackHandler {
  void attachCallback(CallbackEvent c, void (*fn)(void)) {
    switch (c) {
      case CallbackEvent::Asserted:
        assertedFn = fn;
        break;
      case CallbackEvent::Deasserted:
        deassertedFn = fn;
        break;
      case CallbackEvent::Started:
        startedFn = fn;
        break;
      case CallbackEvent::Stopped:
        stoppedFn = fn;
        break;
      default:
        break;
    }
  }

  void runCallback(CallbackEvent c) {
    void (*fnp)(void) = NULL;
    switch (c) {
      case CallbackEvent::Asserted:
        fnp = assertedFn;
        break;
      case CallbackEvent::Deasserted:
        fnp = deassertedFn;
        break;
      case CallbackEvent::Started:
        fnp = startedFn;
        break;
      case CallbackEvent::Stopped:
        fnp = stoppedFn;
        break;
      default:
        break;
    }
    if (fnp != NULL) {
      fnp();
    }
  }

private:
  void (*assertedFn)() = NULL;
  void (*deassertedFn)() = NULL;
  void (*startedFn)() = NULL;
  void (*stoppedFn)() = NULL;
};

struct superFreqCallback {
  void attachCallbackHandler(superFreqCallbackHandler *handler) {
    callbackHandler = handler;
  }

  void runCallback(CallbackEvent e) {
    if (callbackHandler != NULL) {
      callbackHandler->runCallback(e);
    }
  }

private:
  superFreqCallbackHandler *callbackHandler = NULL; /* callback handler */
};

#endif // __SUPER_FREQ_CALLBACK_H__
