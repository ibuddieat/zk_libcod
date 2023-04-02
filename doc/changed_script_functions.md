Changed script code methods:
  * `<entity> setHintString(<string>)` now also supports trigger_radius entities
  * `<player> disableItemPickup()` and `<player> enableItemPickup()` were merged into `<player> itemPickup(<enabled>)`
  * `<player> get_userinfo()` now returns strings only, instead of string or undefined
  * `<player> getClientState()` was renamed to `<player> getClientConnectState()`

Changed script code functions:
  * `bulletTrace(<start>, <end>, <hit characters>, <ignore entity>, [<content mask>])` got one more parameter for custom content masks and now also returns surface flags
  * `getType(<parameter>)` now correctly resolves object types (e.g., struct, array, entity)
  * `obituary(<victim>, <attacker>, <weapon>, <meansOfDeath>, [<team>], [<origin>], [<max. distance>])` got three more parameters
  * `sightTracePassed(<start>, <end>, <hit characters>, <ignore entity>, [<content mask>])` got one more parameter for custom content masks