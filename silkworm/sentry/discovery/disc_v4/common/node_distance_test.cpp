/*
   Copyright 2023 The Silkworm Authors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "node_distance.hpp"

#include <catch2/catch.hpp>

namespace silkworm::sentry::discovery::disc_v4 {

TEST_CASE("distance") {
    auto id = EccPublicKey::deserialize_hex;

    CHECK(node_distance(id("e2b1dfe70675c9c82c540789359013153bd5bba3660956f0eff031e3ae4b5a8bb78e396a9e9f75fb24e9922d5855c675afbf7bbbd0a286b46e7fcb73091735e9"), id("29327274fb7cef377c555ba55e345c0c5c026d145282bbd26f88e0ba174d1a5d83903db9a5ee80a8320b6fc539c7459a574aa95477c7ea4b6cde9fa007bb3bc4")) == 256);
    CHECK(node_distance(id("f6184250f0ca4b0c711889e27874d440c717a1dc3ca79345414bade42b11178b14004cf2bc3fd9a5117eb0bbc675da0bfe150669f58de59f6c8e259c6fdf89be"), id("708ddc2b343a3d2668d4866188545edd96a0c8c2bc6b21daab69f047fda504d60c2d4edb9afa947955c1e7b61756588e484a6379c5885ab1caaf836ee0d6a3a0")) == 255);
    CHECK(node_distance(id("b9b918c8f1198bafbd9a8fa50ad689393c2252eff275fc1490b6ea4993c40f1117ddeafef88ae5a702ef08617d626b3699343ba8b0dba3485b4e8ada74c2f261"), id("ca53d549d6a5ced4d075eb2341df142340ea5c2fe5dd72dd3b12578ee9abf7314be25a5d1bf2dea264ff0bbdd933e1265bdc2f050af2ee723134f5cbb2038e80")) == 254);
    CHECK(node_distance(id("35474838cfaafff7f4a9321415edc7658f46212bef75681824e9bab9f3f3a2c717b522a947c116693e9ac22705d1f15108764ada4962cc770dfbdf34f5bbc479"), id("a3e6f07150e298427a00a80591eda5356cb07344e94a54a42ba5c58ec0e7fd5acc7b897e9c35dbc044f1bfec07fa431d3d3757b6e0939e983b4bfba03f885c7e")) == 253);
    CHECK(node_distance(id("b800d659c02d66f2f8e5347fae0663e772390d0b33da18c393d84834da70b7b8522a0099e85a16275c6cb274227f2f99efee4e13662b73c45a90dfda0ddc27c4"), id("8e9ef977bd93cc76cb62dfc65b1534878b2315271e4c09fc5337e2d672b3220af3a77a33a57a1958c179e2ae8358a4037584a370026215d50a884c06f2fd9cfa")) == 252);
    CHECK(node_distance(id("00fc6ff33b589fc2879905f9a179ee6ab21d80fa4581745bc15c0f311075cec435604a61feffc405445101db0288bb943c43821566a4b8b1aa90117faadccad5"), id("b81a407076268b11790429dbf2bb5e0d466a5606e2549a794409112ebbd24f4361a0c0e2adec0fc43d64de17dbb9a2f3a917dd94f6aecded87cb6f62c6319c57")) == 251);
    CHECK(node_distance(id("42f37f45f0133c951d1cf4ac33cd2feb93893cb7558b534e1b4f96bef95be0245181f1f1fa7e987461561e5bc5d02c8b4f5e2b134b5d2d8c44d711a2c857ef3c"), id("6609dd69289f41c22d6d8338a29e050a77efdad2489caf49cdd5b70d6bdc0366f69ec024bac63d8974844a73b3472e6611f2ea26bf49bb285dfa8fcb5bdd860d")) == 250);
    CHECK(node_distance(id("973e0b58bd1cd8eec974ddb2eda51dad21783295a8ba55a88e1cddbe5072dabcc7db16922361a9e66c154cb71c9aa7bedd5003a356f44916d8b85c1467846a14"), id("87fee2d3f5a6a1b6ac2d003ef82bd691fd828737a198b0dfc483bd89c7317ea67f32851166b218966314ed0e6cc4967c222729415f9be08c806f7d770a4750ec")) == 249);
    CHECK(node_distance(id("b655f1c7987d6b3ee6f96fa89f9802944fedfa44055e920a82d4953a267e1dc61898186277f7fe0cbb4e254c142df4ac08d64981f91087020dbacf7724942749"), id("95f9649da17beb05b6ff00043f64f2ae06c3e5587f461eecc24083d99033fa2c3c1552d38da970a6446f583278edd5c67c7945d9339cbeacafad77aff773c6b3")) == 248);
    CHECK(node_distance(id("51ad4bb22b48246ba0c663b31fa14a1edbc3054cf88136f9a62e52b813f0e39f0bdf4ad4a1a4db6309c8df245a53fe505ac18624a07de4f04ec265de39998507"), id("e5f5648ca4218c0a35758984b5eaf7b9e94c189e982c4fbcdecbb2ef6209885768278fb60f5d8de95416d4b8819da4566f0fcfe9f18147237c8f656bc3666824")) == 247);
    CHECK(node_distance(id("0e695470d54acc038b9c470512541ddd7e2c6e80415b92253a4ac0a8277a970f72add72e8099082544ac7242f5edc61d005af2be514f70376733166a06a0d087"), id("1bfbacf7ccf8bce69087687b9280b0eb74a65a5797977f2f2224052fd7bcc800b5311cacfad7f1e6adf55d5a1681e243528c0127d506c76259d02e8fe9c49732")) == 246);
    CHECK(node_distance(id("34afcbabcb95569e9fde85ff17bdae3d928bd7c625271e7ab047a0a26cd45f3f5e41fbfac88dbbb8e85422328be1e78a30d5957e1c025c97fffc7602dac48baa"), id("dcb371597d681dc7759a153956de93a98cd2ed69e16f9f326c1e36d514e5c92951678eac6a2657921268967a977823443da3a954e610d1cb7512e7dc72e9559d")) == 245);
    CHECK(node_distance(id("0bdb585d1f8561e38d8ba05b5e743abfaa89b9a8dd8367172e4c0f2f413c395d16f1f4eb2b6a065015b40131f885cda85f0b4e12fab83c6cb0406cfcc5de0185"), id("a983e853a3409c2f2861c06bc8266fa9c41f6c253dd219385abeb1be1999336214c60bccbe95446952f81063effc447f0abde249f127922a4d25ed3040a87e5a")) == 244);
    CHECK(node_distance(id("811309d80e1e21f1284ba4fd8d6d0faf81b8d284bcacad34d8c66c501f7792a23e3341235eb294684224a4996e1121de72b6f1f106ccdc6da3994753fbf317ef"), id("a7dab310c77f6dc0e559f69df6c6af4a7661512658d44fd269dd8882d36ffef6c30c4f450df07eab7a32e38df186d4ac659e5b889d5e10ab292d1e2c0809ea00")) == 243);
    CHECK(node_distance(id("c4ee97740ee9733d049c157ef34dbd3e0bdd4e2b6e3fc5d281eae03eeecea78d2b84cb8b5eaa48f8af721f3665fdbeb22471732f1c671ae3675b23c0663990df"), id("779ad287894ef641131ac6dc2dcd3a5a976650cfad9d8396c1c7184e133899b2e17ca2505838754b06774f74551cc3f8f0e24e5f4fe2d9f5f4bd711b1f4290ea")) == 242);
    CHECK(node_distance(id("8b2d08a040bdb1bd3bc1d947e98edf760ceef8506e1c6f7d0b30fce6fbe4f419a61fe6726dc166416eb4ed12f8f3063ccc0580600052b519d27336084f75b1ee"), id("e99292b9cbdab866b1e7eca5081a1ea2fcdb0af5da77d5fffae5fcc7fbc985394bebd0b0fea3428e9164602d573c10628097d7b2304dc40c4133bcaa775a6433")) == 241);
    CHECK(node_distance(id("edd26fcfad2d3f68f4c8ba7018895708909a87a1f3b34f0f931c6b8ab5a57c2264483cdfa0c910c597605736c946696703af752a93f3f03f14b8e1ffbcf92855"), id("3aa6608c6ac787577f628f7c7fcc179ea9a5c9a6a54bf50146d44ee3e2425de2465aacadb97057d4f8d26127c9997d292bed8ff441d91251dc1640d743f65cdb")) == 240);
    CHECK(node_distance(id("e31da2ba534ebdfbe8b1863c5a76eaa3dda03c6a55f7345e6a9ef1cda3c3ba77ae03513d37903c44b5400b0a1fbce6f6d87ec810563a6563794a91d2631f8f09"), id("3b5d897d76523a15a84ac1fef4dae83a2bce6501fde29ae620749a35dc1199d12666fdfef148ebd9bfc2cf2f25af9820049e34472612934d91603ab05278efd5")) == 239);
    CHECK(node_distance(id("cfd448d68854f5b8031d8ab13fdad34ff46b6a83c39711c58cb31b54c9b1798c6aa167adf9d3dae3e852a9569c2ee3a21dbb93a5441e5ee836554bf4697dec4f"), id("12743e04f81934681dc9a2da498fa574e7b95851bc9ed4903dc37be7e6171319815165bed420f0b6e4cae4923fb62f37932bee553f3d8b92dd4e7bbad469aa8e")) == 238);

    CHECK(node_distance(id("cfd448d68854f5b8031d8ab13fdad34ff46b6a83c39711c58cb31b54c9b1798c6aa167adf9d3dae3e852a9569c2ee3a21dbb93a5441e5ee836554bf4697dec4f"), id("cfd448d68854f5b8031d8ab13fdad34ff46b6a83c39711c58cb31b54c9b1798c6aa167adf9d3dae3e852a9569c2ee3a21dbb93a5441e5ee836554bf4697dec4f")) == 0);
    CHECK(node_distance(id("d00f5c3209c00b5f40300b0613329b2e65de285ac7b0c7e36d9bfbf64715a23b85380a3657514a71eeec54d0d585cda04aa1b618265a62c8798a2eba7ce16f0d"), id("d00f5c3209c00b5f40300b0613329b2e65de285ac7b0c7e36d9bfbf64715a23b85380a3657514a71eeec54d0d585cda04aa1b618265a62c8798a2eba7ce16f0d")) == 0);

    CHECK(node_distance(id("d00f5c3209c00b5f40300b0613329b2e65de285ac7b0c7e36d9bfbf64715a23b85380a3657514a71eeec54d0d585cda04aa1b618265a62c8798a2eba7ce16f0d"), id("da06206e55dd849c7c2c0b97def4b0e62bf99d7f395a69b5efe370dc5e501636039c41bd52c4b9d7a4d409d9b59aeb66bd2f25d0db85a1cf84889a0b2893b755")) == 256);
    CHECK(node_distance(id("40587ed129df02457b35027564ed7a915e8638460f3a1e55a04c29d447a36b731d5105be6f0317b6d4b231bd9a3e974d42822c27ce16e74b258fb8642dcd8f43"), id("19359199e6aa0038bca8e440e441dbd86b523ecaca5e0969bd80ac13ee75137dd43c4fe59907cba35ea723b1d3fe448401bfc4f8a4eee7fcf4b468e6dfbc1fa2")) == 256);
    CHECK(node_distance(id("1f20d6dff3a573aa8516d4ae266de6b0e0b0b371fbc45c7570e230f3559a9636bcf6cfd9b68c1b8cbb7f865c63dad3029feace82a5c206cce7c6699bc8c7c90e"), id("7abdedc2bc730891a0c5d25f9db3e77772531552ae1ca60d6fcf52110cc00efc2a41beb313d8d6a486c6b916638b43c843e0c15e1da27969157c9652f05584e8")) == 255);
    CHECK(node_distance(id("27a3e8a9bd8c3dc8d8385a95da26fe07cfdabf520d6516688e5d1f58009eba25ca219b0f647b9912ec7f215fb6b628c5eb6f10dfa60f213b7e623081fa96864b"), id("6047670f2a064f12681f7e850336a606000bb5287cb2631d3c469d0add1f535e29883b8b7523b5caf095b30456f9548b19dceacbbcd8a4532468c40c4d7cd14b")) == 255);
    CHECK(node_distance(id("8606e2e73bb7120b3c8a280bc9ddb6ec103042109da0a50f2a991ca19502b2225186d64ab1d26ad9229d9181ccfb54fdbfff54f9209147751388eeb135372ce6"), id("4f02bc52c7a8692bba40675b5e955ad6411344dbd3e6a2015947c54193d54c61bde8dc66ab0bec477e3baf53c193c425d28af599cd41129ab3a91d1ac32c0a4e")) == 254);
    CHECK(node_distance(id("905c5bdd20fc03e05042bdfac884336852077a054da47cfdf6fe763ae0774f9f0bfe0e4bba40738d5810418876183e9c9c17e25579a1e742fe09022a8f7d26c4"), id("d03349d73725e3b967e1edb2c9fa0c4f8f56e2d522a1289e889c5766d8f4e33408b0f363602eb3dcec53c7c8ea047d691994a1e2abb50da8bc8aba5fa4ab18ae")) == 254);
    CHECK(node_distance(id("b4cb19f92fb2325e41f239f7ed2e44da26e0578add3e3682f1dad0a94ce14d4e36bf092e6f9717b68765a76552a5b403f78f3f5c92c0f8172edd63c67e369f79"), id("62beb0427661dd95e24329ca34d5791db678a5c31c8bb9a2de81e464fe97a63632944c2b9ab07a9c4441b2adddabcf5c35c73fbfae483c1b416d8cfb3826d841")) == 254);
    CHECK(node_distance(id("76fbc4bb1a26eda04237fcdd9042c04e52c9c2302f32d455fe75479f54a441251e8e7e003729c9014611b710c5916e63ae920b5dfa6901692f5b7e82621cd528"), id("7764238559b9541c75ab34eb6c923b6e71c5d10291933f84c50b65575371377b9a14d7c0eb5e94e0ee5e5cb42347a149a734a18e42b49cd477c86701c1667a97")) == 253);
    CHECK(node_distance(id("f23636b28840898bc50c19897c1f64528e25331d47148806d662919b206d69da9dce74f97adb6f7925e8ca2172d5ee7f72bf7a82b1cb688b1376746c6fd9c293"), id("3656d2b5b4a80592dc8484d821bdd7b0e131c93b7425d61f37c4273ceb8e078f63b420703445b41f5123d4b6978950ff602fe792ea50c50f43cff6e7f2971e87")) == 252);
    CHECK(node_distance(id("455920ca5e232c195e517095c0bb6fbd009b0271d3e3302b1ee743f948256a47f267fb68b4b5abf484ff4b1319c792801760a17edc995d764f6a5f748c71ba34"), id("22916cb121608c65cd8604744b9aaab267830f0d23ef36af05c0d5375e65e5fde8c4238d25b708d2b3e22d0142774ac8875dcb00e512b63f29136965e25c84df")) == 251);
}

}  // namespace silkworm::sentry::discovery::disc_v4
