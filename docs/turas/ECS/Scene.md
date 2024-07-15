A scene is a collection of all active entities and associated components (data) that forms a scene within the engine.

Many scenes may be active at ones and all active scenes will be processed each frame, though a stagger mechanism will likely be added. 

Scenes are saved in a binary format. When loaded, all the required [[Asset]]s are first enqueued to the [[Asset Manager]]. Once all assets have finished loading, the scene is created and deserialized. populated with associated [[Asset]]s where necessary.