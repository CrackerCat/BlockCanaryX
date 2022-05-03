package blockcanary

import android.app.Application
import android.content.Context
import androidx.startup.Initializer

internal class BlockCanaryStartupInitializer : Initializer<BlockCanaryStartupInitializer> {


    override fun create(context: Context)= apply {
        val application = context.applicationContext as Application
        application.applicationInfo

        val autoInstall = application.applicationInfo.metaData?.getBoolean("block_canary_auto_install", true) ?:true
        if (autoInstall){
            val blockCanaryConfig = BlockCanaryConfig.newBuilder().build()
            BlockCanary
                .install(application,blockCanaryConfig)
        }
    }

    override fun dependencies()= emptyList<Class<out Initializer<*>>>()
}