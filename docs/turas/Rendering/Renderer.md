Renderer contains all active [[View]]s, all [[Shaders]] and [[Pipeline]] creation templates. 

Renderer is responsible for destroying and recreating [[View]] when a shader has changed, notified by hot reload sources and completely disabled in runtime builds

Renderer contains the global instance of [[LVK]]

