# JSON Campaign Level System

`levels/campaign_levels.json` is the campaign source of truth. It supports up to 32 levels with fixed arrays, so no STL containers, templates, enums, or external JSON libraries are used.

Every level needs a stable `id`, contiguous `order`, act/level metadata, `structureType`, `gameplayType`, display text, and logical map arrays. The current supported gameplay identifiers are lowercase `snake_case` strings and are validated when the campaign loads.

`LevelManager` loads the JSON, validates IDs/orders/types, calculates `optimalSteps`, derives `alarmCapacity` with `(3 * optimalSteps + 1) / 2 + 2`, then creates the active score state. Authored JSON must not contain authoritative `optimalSteps` or `alarmCapacity` values.

Optimal actions come from the configured map: sequence-style levels count required values; BST search/insert use their configured required route; staged deletion counts configured stages plus the required successor decision; the degeneration boss sums the insertion route costs; order-statistic levels count repair plus configured search route; heap build simulates bottom-up sift decisions on a temporary fixed array.

Campaign progress is saved in `VAULT_HEIST_SAVE_V2` by level ID. Older V1 index-based saves are intentionally ignored, producing a fresh campaign as agreed.
