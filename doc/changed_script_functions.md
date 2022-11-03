Changed script code functions:
  * `<entity> setHintString(<string>)` now also supports trigger_radius entities
  * `<player> get_userinfo()` now returns strings only, instead of string or undefined
  * `<player> getClientState()` was renamed to `<player> getClientConnectState()`
  * `obituary(<victim>, <attacker>, <weapon>, <meansOfDeath>, [<team>], [<origin>], [<max. distance>])` got three more parameters
  * `bulletTrace(<start>, <end>, <hit characters>, <ignore entity>, [<content mask>])` got one more parameter