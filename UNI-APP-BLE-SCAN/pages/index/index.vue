<template>
	<view class="content">
		<u-notify ref="uNotify"></u-notify>
		<view class="body-contariner">
			<view class="body-title">
				<p>传感器状态</p>
			</view>
			<view class="dashboard">
				<best-gauge :config="gaugeOption"></best-gauge>
			</view>
			<view class="item-cards">
				<view class="item-card">
					<u-icon name="tags-fill" color="#16dc00" size="30"></u-icon>
					<p>Device Name:</p>
					<p class="value">{{Name}}</p>
				</view>
				<view class="item-card">
					<u-icon name="tags-fill" color="#16dc00" size="30"></u-icon>
					<p>Tem Payload:</p>
					<p class="value">{{Temp}}</p>
				</view>
				<view class="item-card">
					<u-icon name="tags-fill" color="#16dc00" size="30"></u-icon>
					<p>Hum Payload:</p>
					<p class="value">{{Hum}}</p>
				</view>
				<view class="item-card">
					<u-icon name="eye-fill" color="#16dc00" size="30"></u-icon>
					<p>UUID:</p>
					<p class="value">{{UUID}}</p>
				</view>
			</view>
		</view>
	</view>
</template>

<script>
	import bestGauge from '../../components/best-gauge/best-gauge.vue'
	export default {
		components: {
			bestGauge
		},
		data() {
			return {
				Name: 'scan....',
				UUID: 'scan....',
				Temp: "Loading....",
				Hum: "Loading....",
				bleEnable: true,
				gaugeOption: { //定义参数
					id: 'gaugeId0',
					value: 0,
					axisTickLength: 2, //该属性与axisTick互斥，存在axisTickLength并大于0，axisTick就无效,
					status: false,
					width: 240,
					height: 240
				},
				timeroutFlag: 0
			}
		},
		onLoad() {

		},
		mounted() {
			this.open()
		},
		onReady() {},
		methods: {
			bleChange() {
				if (this.bleEnable) {
					this.open()
				} else {
					this.close()
				}
			},
			u8a2str(fileData) {
				var dataString = "";
				for (var i = 0; i < fileData.length; i++) {
					dataString += String.fromCharCode(fileData[i]);
				}
				return dataString
			},
			ab2hex(buffer) {
				const hexArr = Array.prototype.map.call(
					new Uint8Array(buffer),
					function(bit) {
						return ('00' + bit.toString(16)).slice(-2)
					}
				)
				return hexArr.join('')
			},
			open() {
				uni.openBluetoothAdapter({
					success: e => {
						console.log('初始化蓝牙成功:' + e.errMsg)
						console.log(JSON.stringify(e))
						// console.log(this.getBluetoothAdapterState())
						this.scan()
					},
					fail: e => {
						console.log(e)
						this.$refs.uNotify.show({
							top: 10,
							type: 'error',
							color: '#ffffff',
							bgColor: '#e85456',
							message: '请检查蓝牙是否开启，并重启APP',
							duration: 1000 * 3,
							fontSize: 20,
							safeAreaInsetTop: true
						})
						console.log('初始化蓝牙失败，错误码：' + (e.errCode || e.errMsg))
						if (e.errCode !== 0) {
							initTypes(e.errCode, e.errMsg)
						}
					}
				})
			},
			close() {
				uni.closeBluetoothAdapter({
					success: e => {
						console.log('蓝牙资源释放:' + e.errMsg)
					},
				})
			},
			stop() {
				uni.stopBluetoothDevicesDiscovery({
					success(res) {
						console.log(res)
						console.log("停止扫描");
					}
				})
			},
			scan() {
				console.log("开始扫描");
				uni.onBluetoothDeviceFound((devices) => {
					if (devices.devices[0].localName == "ESP32_SHT30") {
						console.log(devices.devices[0].advertisServiceUUIDs)
						console.log(this.ab2hex(devices.devices[0].advertisData))
						this.UUID = devices.devices[0].advertisServiceUUIDs[0]
						try {
							this.Name = devices.devices[0].localName
							let rawStr = String.fromCharCode.apply(null, new Uint8Array(devices.devices[0]
								.advertisData))
							console.log(rawStr)
							let data = JSON.parse(rawStr)
							console.log(data)
							if (data.Temp || data.Hum) {
								this.gaugeOption.status = true
								let key = Object.keys(data)[0]
								this[key] = rawStr
								if (key == "Temp") {
									this.gaugeOption.value = data.Temp
								}
								if (this.timeroutFlag) {
									clearTimeout(this.timeroutFlag);
								}
								this.timeroutFlag = setTimeout(() => {
									this.gaugeOption.status = false
								}, 4000);
							}
						} catch (e) {
							console.log(e)
						}
						console.log()
					}
				})

				uni.startBluetoothDevicesDiscovery({
					services: ["C201"],
					powerLevel: "high",
					allowDuplicatesKey: true,
					success(res) {
						console.log(res)
					}
				})
			}
		}
	}
</script>

<style lang="scss">
	.content {
		display: flex;
		flex-direction: column;
		align-items: center;
		justify-content: center;
		height: 100%;
		background-color: #242424;
	}

	.body-contariner {
		display: flex;
		flex-direction: column;
		justify-content: space-between;
		align-items: center;
		height: 100%;
		width: 100%;

		.body-title {
			color: white;
			height: 180rpx;
			line-height: 180rpx;
			margin-top: 50rpx;
			font-weight: bold;
		}

		.item-cards {
			display: flex;
			flex-direction: column;
			padding: 10rpx;
			width: 100%;

			.item-card {
				display: flex;
				padding: 0rpx 30rpx;
				height: max-content;
				justify-content: left;
				min-height: 120rpx;
				margin: 10rpx;
				background-color: #ffffff12;
				border-radius: 5rpx;
				align-items: center;
				color: white;
				font-size: 22rpx;

				.u-icon {
					margin-right: 10rpx;
				}

				.value {
					margin-left: 30rpx;
					color: #16dc00;
					word-break: break-all;
				}

			}
		}

		.dashboard {
			margin: 52rpx 0 80rpx 0;
			padding: 40rpx;
			border-radius: 50%;
			overflow: hidden;
		}
	}
</style>
